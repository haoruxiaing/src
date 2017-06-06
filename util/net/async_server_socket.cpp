#include "async_server_socket.h"

#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>

const uint32_t AsyncServerSocket::kDefaultMaxAcceptAtOnce;

static int SetCloseOnExec(int fd) {
    int old_flags = ::fcntl(fd, F_GETFD, 0);

    if (old_flags < 0) 
        return -1;

    int new_flags = old_flags | FD_CLOEXEC;
    return ::fcntl(fd, F_SETFD, new_flags);
}

AsyncServerSocket::AsyncServerSocket(EventLoop* evl) 
    : evl_(evl)
    , server_handler_(evl, -1, this)
    , max_accept_at_once_(kDefaultMaxAcceptAtOnce)
    , accepting_(false)
    , accept_callback_(0) {

}

AsyncServerSocket::~AsyncServerSocket() {
}

bool AsyncServerSocket::Bind(const SocketAddress& address) {
    int fd = CreateSocket();
    int rs = -1;
    if (fd >= 0) {
        rs = BindSocket(fd, address);
    }

    return (fd >= 0 && rs == 0) ? true : false;
}

bool AsyncServerSocket::Bind(uint16_t port) {
    SocketAddress address(port);
    return Bind(address);
}

void AsyncServerSocket::GetAddress(SocketAddress* address) {
    address->SetFromPeerAddress(server_handler_.socket_);
}

bool AsyncServerSocket::Listen() {
    int rs = ::listen(server_handler_.socket_, SOMAXCONN);
    if (rs == -1) {
        LOG(ERROR) << "failed in listen: " << strerror(errno);
        return false;
    }
    return true;
}

void AsyncServerSocket::StartAccepting() {
    accepting_ = true;
    server_handler_.RegisterHandler(EventHandler::READ);
}

void AsyncServerSocket::StopAccepting() {
    accepting_ = false;
    server_handler_.UnregisterHandler();
    ::close(server_handler_.socket_);
}

void AsyncServerSocket::SetAcceptCallback(AcceptCallback* callback) {
    accept_callback_ = callback;
}

void AsyncServerSocket::RemoveAcceptCallback() {
    accept_callback_ = 0;
}

void AsyncServerSocket::HandlerReady(uint16_t events, int fd){
    (void)events;
    for (uint32_t n = 0; n < max_accept_at_once_; ++n) {
        sockaddr_storage addr_storage;
        socklen_t addr_len = sizeof(addr_storage);
        sockaddr* saddr = reinterpret_cast<sockaddr*>(&addr_storage);
        int client_socket = ::accept(fd, saddr, &addr_len);
        
        if (client_socket < 0) {
            if (errno == EAGAIN) {
                return ;
            } else if (errno == EMFILE || errno == ENFILE) {
                LOG(ERROR) << "accept failed: out of file descriptors";
            } else {
                LOG(ERROR) << "accept() failed, errno=" << errno;
            }
            return ;
        }

        if (accept_callback_ == 0) {
            ::close(client_socket);
            LOG(ERROR) << "accept_callback_ not set, close socket: " << client_socket;
            return ;
        }

        if (::fcntl(client_socket, F_SETFL, O_NONBLOCK) != 0) {
            ::close(client_socket);
            LOG(ERROR) << "failed to set accepted socket to non-blocking mode" 
                       << ", errno=" << errno;
            return ;
        }

        SocketAddress address;
        address.SetFromPeerAddress(client_socket);

        accept_callback_->ConnectionAccepted(client_socket, address);
    }
}

int AsyncServerSocket::CreateSocket() {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        LOG(ERROR) << "error creating async server socket, errno=" << errno;
        return fd;
    }

    int rs = SetupSocket(fd);
    return rs == 0 ? fd : rs;
}

int AsyncServerSocket::SetupSocket(int fd) {
    int result = -1;
    do {
        if (::fcntl(fd, F_SETFL, O_NONBLOCK) != 0) {
            LOG(ERROR) << "failed to put socket in non-blocking mode, errno="
                       << errno;
            return -1;
        }

        int one = 1;
        if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) != 0) {
            LOG(ERROR) << "failed to set SO_REUSEADDR on async server socket,"
                       << "errno=" << errno;
            break;
        }

        if (::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one)) != 0) {
            LOG(ERROR) << "failed to set SO_KEEPALIVE on async server socket: "
                       << strerror(errno);
            break;
        }

        if (SetCloseOnExec(fd) == -1) {
            LOG(ERROR) << "failed to set FD_CLOEXEC on async server socket: "
                       << strerror(errno);
            break;
        }

        if (::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one)) != 0) {
            LOG(ERROR) << "failed to set TCP_NODELAY on async server socket: "
                       << strerror(errno);
            break;
        }
        result = 0;
    } while (0);

    return result;    
}

int AsyncServerSocket::BindSocket(int fd, const SocketAddress& address) {
    sockaddr_in addr = address.addr();
    if (::bind(fd, (struct sockaddr*)&addr, 
               static_cast<socklen_t>(sizeof(struct sockaddr))) != 0) {
        ::close(fd);
        LOG(ERROR) << "failed to bind to async server socket: " 
                   << strerror(errno) << ", errno=" << errno;
        return -1;
    }

    server_handler_.InitHandler(evl_, fd);
    return 0;
}
