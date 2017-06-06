#include "async_socket.h"

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#include "event_loop.h"
#include "event_handler.h"
#include "buffer.h"
#include "socket_address.h"
#include "async_timeout.h"

AsyncSocket::AsyncSocket(EventLoop* evl) 
    : evl_(evl)
    , readtimeout_(this, evl)
    , io_handler_(this, evl, -1) {
    Init();
}

AsyncSocket::AsyncSocket(EventLoop* evl, int fd)
    : evl_(evl)
    , readtimeout_(this, evl)
    , io_handler_(this, evl, fd) {
    Init();
    fd_ = fd;
    SetCloseOnExec();
    state_ = StateEnum::ESTABLISHED;
}

AsyncSocket::AsyncSocket(EventLoop* evl,
                         const SocketAddress& address,
                         uint32_t connect_timeout)
    : fd_(-1) 
    , evl_(evl)
    , readtimeout_(this, evl)
    , io_handler_(this, evl, -1) {
    Init();
    Connect(0, address, connect_timeout);
}

AsyncSocket::AsyncSocket(EventLoop* evl,
                         const std::string& ip,
                         uint16_t port,
                         uint32_t connect_timeout) 
    : fd_(-1) 
    , evl_(evl)
    , readtimeout_(this, evl) 
    , io_handler_(this, evl,-1) {
    Init();
    Connect(0, ip, port, connect_timeout);
}

AsyncSocket::~AsyncSocket() {
    readtimeout_.CancelTimeout();
    io_handler_.UnregisterHandler();
    connect_callback_ = 0;
    read_callback_ = 0;
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

void AsyncSocket::Connect(ConnectCallback* callback,
                          const SocketAddress& address,
                          uint32_t timeout) {
    assert(evl_->IsInEventLoopThread());

    addr_ = address;

    // 确保在未初始化状态
    if (state_ != StateEnum::UNINIT) {
        if (callback) {
            callback->ConnectErr();
        }
        return ;
    }

    assert(fd_ == -1);
    state_ = StateEnum::CONNECTING;
    connect_callback_ = callback;

    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ < 0) {
        ::abort();
    }
    io_handler_.ChangeHandlerFD(fd_);

    SetCloseOnExec();

    // set nonblock
    int flags = ::fcntl(fd_, F_GETFL, 0);
    if (flags == -1) {
        ::abort();
    }
    int rs = ::fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
    if (rs == -1) {
        ::abort();
    }

    // By default, Turn on TCP_NODELAY
    (void)SetNoDelay(true);

    sockaddr_in saddr = addr_.addr();
    socklen_t saddr_len = static_cast<socklen_t>(sizeof(saddr));
    rs = ::connect(fd_, sockaddr_cast(&saddr), saddr_len);
    if (rs < 0) {
        if (errno == EINPROGRESS) {
            if (timeout > 0) {
                readtimeout_.ScheduleTimeout(timeout);
            }
        }

       assert(event_flags_ == EventHandler::NONE);
       event_flags_ = EventHandler::EventFlags::WRITE;
       io_handler_.RegisterHandler(event_flags_);
       return ;
    }

    // For here, connected success.
    assert(read_callback_ == 0);
    state_ = StateEnum::ESTABLISHED;
    if (callback) {
        connect_callback_ = 0;
        callback->ConnectSuccess();
    }
}

void AsyncSocket::Connect(ConnectCallback* callback,
                          const std::string& ip,
                          uint16_t port,
                          uint32_t timeout) {
    Connect(callback, SocketAddress(ip, port), timeout);
}

void AsyncSocket::CancelConnect() {
    connect_callback_ = 0;
    if (state_ == StateEnum::CONNECTING) {
        CloseNow();
    }
}

void AsyncSocket::SetReadCB(ReadCallback* callback) {
    if (callback == read_callback_) {
        return ;
    }

    assert(evl_->IsInEventLoopThread());
    switch (state_) {
        case StateEnum::CONNECTING:
            read_callback_ = callback;
            return;
        case StateEnum::ESTABLISHED: 
        {
            read_callback_ = callback;
            uint16_t old_flags = event_flags_;
            if (read_callback_) {
                event_flags_ |= EventHandler::READ;
            } else {
                event_flags_ &= ~EventHandler::READ;
            }
            if (event_flags_ != old_flags) {
                UpdateEventRegistration();
            }
            return ;
        }
        case StateEnum::CLOSED:
        case StateEnum::ERROR:
        case StateEnum::UNINIT:
            //ingore
            return;
    }
}

void AsyncSocket::Write(const char* buf, size_t len) {
    Write(std::string(buf, len));
}

void AsyncSocket::Write(const std::string& message) {
    assert(evl_->IsInEventLoopThread());
    if (state_ == StateEnum::ESTABLISHED) {
        PerformWrite(message);
    }
}

void AsyncSocket::Close() {
    if ((output_buffer_.ReadableBytes() == 0) ||
        !(state_ == StateEnum::CONNECTING ||
        state_ == StateEnum::ESTABLISHED)) {
        CloseNow();
        return ;
    }

    assert(evl_->IsInEventLoopThread());
    shutdown_flags_ |= (SHUT_READ | SHUT_WRITE_PENDING);
    if (read_callback_) {
        UpdateEventRegistration(0, EventHandler::READ);
        ReadCallback* callback = read_callback_;
        read_callback_ = 0;
        callback->ReadEOF();
    }
}

void AsyncSocket::CloseNow() {
    assert(evl_->IsInEventLoopThread());

    switch (state_) {
        case StateEnum::ESTABLISHED:
        case StateEnum::CONNECTING: {
            shutdown_flags_ |= (SHUT_READ | SHUT_WRITE);
            state_ = StateEnum::CLOSED;

            // If the read timeout was set, cancel it.
            readtimeout_.CancelTimeout();

            // If we are registered for I/O events, unregister.
            if (event_flags_ != EventHandler::EventFlags::NONE) {
                event_flags_ = EventHandler::EventFlags::NONE;
                UpdateEventRegistration();
            }

            if (connect_callback_) {
                ConnectCallback* callback = connect_callback_;
                connect_callback_ = 0;
                callback->ConnectErr();
            }

            if (read_callback_) {
                ReadCallback* callback = read_callback_;
                read_callback_ = 0;
                callback->ReadEOF();
            }

            if (fd_ >= 0) {
                ::close(fd_);
                fd_ = -1;
            }

            return;
        }
        case StateEnum::CLOSED:
        case StateEnum::ERROR:
            return;
        case StateEnum::UNINIT:
            assert(event_flags_ == EventHandler::EventFlags::NONE);
            assert(connect_callback_ == 0);
            assert(read_callback_ == 0);
            shutdown_flags_ |= (SHUT_READ | SHUT_WRITE);
            state_ = StateEnum::CLOSED;
            return;
    }
}

void AsyncSocket::GetLocalAddress(SocketAddress* address) {
    address->SetFromLocalAddress(fd_);
}

void AsyncSocket::GetPeerAddress(SocketAddress* address) {
    address->SetFromPeerAddress(fd_);
}

int AsyncSocket::SetNoDelay(bool nodelay) {
    if (fd_ < 0) {
        return EINVAL;
    }

    int value = nodelay ? 1 : 0;
    if (::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value)) != 0) {
        int errno_copy = errno;
        return errno_copy;
    }
    return 0;
}

void AsyncSocket::SetCloseOnExec() {
    int rs = ::fcntl(fd_, F_SETFD, FD_CLOEXEC);
    if (rs != 0) {
        //LOG(FATAL) << "failed to set close-on-exec flag, errno: " << errno;
    }
}

int AsyncSocket::SetSendBufSize(size_t bufsize) {
    if (fd_ < 0) {
        return EINVAL;
    }

    if (::setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize)) != 0) {
        int errno_copy = errno;
        return errno_copy;
    }

    return 0;
}

int AsyncSocket::SetRecvBufSize(size_t bufsize) {
    if (fd_ < 0) {
        return EINVAL;
    }

    if (::setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize)) != 0) {
        int errno_copy = errno;
        return errno_copy;
    }
    return 0;
}

void AsyncSocket::Init() {
    assert(evl_ == 0 || evl_->IsInEventLoopThread());
    state_ = StateEnum::UNINIT;
    shutdown_flags_ = 0;
    event_flags_ = EventHandler::EventFlags::NONE;
    fd_ = -1;
    read_timeout_ = 0;
    connect_callback_ = 0;
    read_callback_ = 0;
}

void AsyncSocket::IoReady(uint16_t events) {
    assert(events & EventHandler::EventFlags::READ_WRITE);
    assert(evl_->IsInEventLoopThread());
    uint16_t relevant_events = events & EventHandler::EventFlags::READ_WRITE;
    if (relevant_events == EventHandler::EventFlags::READ) {
        HandleRead();
    } else if (relevant_events == EventHandler::EventFlags::WRITE) {
        HandleWrite();
    } else if (relevant_events == EventHandler::EventFlags::READ_WRITE) {
        HandleWrite();
        // 只有设置了read callback才调用HandleRead().
        // 在HandleWrite()回调中, 有可能未设置read callback.
        if (read_callback_) {
            HandleRead();
        }
    } else {
        ::abort();
    }
}

void AsyncSocket::HandleRead() {
    assert(state_ == StateEnum::ESTABLISHED);
    assert(read_callback_ != 0);
    assert(event_flags_ & EventHandler::EventFlags::READ);

    ssize_t bytes_read = PerformRead();
    if (bytes_read > 0) {
        read_callback_->ReadDataAvailable(&input_buffer_);
    } else if (bytes_read < 0) {
        if (errno != EAGAIN || errno != EWOULDBLOCK) {
            shutdown_flags_ |= SHUT_READ;
            UpdateEventRegistration(0, EventHandler::EventFlags::READ);
            if (read_callback_) {
                ReadCallback* callback = read_callback_;
                read_callback_ = 0;
                callback->ReadErr(errno); 
            }
        }
    } else {
        assert(bytes_read == 0);
        shutdown_flags_ |= SHUT_READ;
        UpdateEventRegistration(0, EventHandler::EventFlags::READ);
        if (read_callback_) {
            ReadCallback* callback = read_callback_;
            read_callback_ = 0;
            callback->ReadEOF();
        }
    }
}

void AsyncSocket::HandleWrite() {
    if (state_ == StateEnum::CONNECTING) {
        HandleConnect();
        return ;
    }

    assert(state_ == StateEnum::ESTABLISHED);

    ssize_t bytes_write = ::write(fd_, 
                                  output_buffer_.Peek(), 
                                  output_buffer_.ReadableBytes());
    if (bytes_write > 0) {
        output_buffer_.Retrieve(bytes_write);
        // if we have nothing to send, unregister write event.
        if (output_buffer_.ReadableBytes() == 0) {
            UpdateEventRegistration(0, EventHandler::EventFlags::WRITE);
        }
    } else {
        if (errno != EWOULDBLOCK) {
            UpdateEventRegistration(0, EventHandler::EventFlags::WRITE);
        }
    }

    // 如果设置了SHUT_WRITE_PENDING, 那么在socket发送完数据后要关闭socket.
    if (shutdown_flags_ & SHUT_WRITE_PENDING) {
        shutdown_flags_ |= SHUT_WRITE;
        if (shutdown_flags_ & SHUT_READ) {
            state_ = StateEnum::CLOSED;
            if (fd_ >= 0) {
               ::close(fd_);
               fd_ = -1;
            } 
        } else {
            ::shutdown(fd_, SHUT_WR);
        }
    }
}

void AsyncSocket::HandleConnect() {
    assert(state_ == StateEnum::CONNECTING);
    readtimeout_.CancelTimeout();
    assert(event_flags_ == EventHandler::EventFlags::WRITE);
    event_flags_ = EventHandler::NONE;
    
    // 调用getsockopt()检查是否连接成功
    int error;
    socklen_t len = sizeof(error);
    int rs = ::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &error, &len);
    if (rs != 0) {
        state_ = StateEnum::ERROR;
        UpdateEventRegistration(0, EventHandler::WRITE);
        ::close(fd_);
        fd_ = -1;
        if (connect_callback_ != 0) {
            ConnectCallback* callback = connect_callback_;
            connect_callback_ = 0;
            callback->ConnectErr();
            return ;
        }
    }

    if (error != 0) {
        state_ = StateEnum::ERROR;
        UpdateEventRegistration(0, EventHandler::WRITE);
        ::close(fd_);
        fd_ = -1;
        if (connect_callback_ != 0) {
            ConnectCallback* callback = connect_callback_;
            connect_callback_ = 0;
            callback->ConnectErr();
            return ;
        }
    }

    // 连接成功
    state_ = StateEnum::ESTABLISHED;
    if (connect_callback_) {
        ConnectCallback* callback = connect_callback_;
        connect_callback_ = 0;
        callback->ConnectSuccess();
    }

   HandleInitialReadWrite(); 
}

void AsyncSocket::TimeoutExpired() {
    assert(evl_->IsInEventLoopThread());

    if (state_ == StateEnum::CONNECTING) {
        // connect timeout 
        state_ = StateEnum::ERROR;
        shutdown_flags_ |= (SHUT_READ | SHUT_WRITE);

        if (event_flags_ != EventHandler::NONE) {
            event_flags_ = EventHandler::NONE;
            io_handler_.UnregisterHandler();
        }
        readtimeout_.CancelTimeout();

        if (fd_ == -1) return;
        ::close(fd_);
        fd_ = -1;

        if (connect_callback_ != 0) {
            ConnectCallback* callback = connect_callback_;
            connect_callback_ = 0;
            callback->ConnectErr();
        }
    } else {
        // a normal read operation timeout
        if (read_callback_ != 0) {
            read_callback_->ReadTimeout();
        }
    }
} 

void AsyncSocket::HandleInitialReadWrite() {
    // If we have a read_callback_, make sure we enable read event.
    if (read_callback_ && !(event_flags_ & EventHandler::EventHandler::READ)) {
        UpdateEventRegistration(EventHandler::READ, 0);
    } else if (read_callback_ == 0) {
        UpdateEventRegistration(0, EventHandler::READ);
    }

    // If we don't have pending write, unregister for write event.
    if (output_buffer_.ReadableBytes() == 0) {
        UpdateEventRegistration(0, EventHandler::WRITE);
    }
}

ssize_t AsyncSocket::PerformRead() {
    return input_buffer_.ReadFd(fd_);
}

void AsyncSocket::PerformWrite(const std::string& message) {
    // If we are not writing, write the message immediately; otherwise put
    // message into output_buffer_.
    assert(state_ == StateEnum::ESTABLISHED);
    assert(evl_->IsInEventLoopThread());

    size_t remaining = message.size();
    ssize_t nwrote = 0;
    bool fault_error = false;

    if (!(event_flags_ & EventHandler::WRITE)) {
        nwrote = ::write(fd_, message.data(), message.size());
        if (nwrote >= 0) {
            remaining = message.size() - nwrote;
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                if (errno == EPIPE || errno == ECONNRESET) {
                    fault_error = true;
                }
            }
            LOG(ERROR) << "AsyncSocket::PerformWrite(this=" << this 
                       << ", fd=" << fd_
                       << ", state=" << state_
                       << ", event=" << event_flags_
                       << ", shutdown_flags=" << std::hex << shutdown_flags_
                       << ") write() failed, errno[" << errno 
                       << "]: " << strerror(errno);
        }
    }

    if (!fault_error && remaining > 0) {
        output_buffer_.Append(message.c_str()+nwrote, remaining);
        if (!(event_flags_ & EventHandler::WRITE)) {
            UpdateEventRegistration(EventHandler::WRITE, 0);
        }
    }
}

void AsyncSocket::UpdateEventRegistration() {
   assert(evl_->IsInEventLoopThread());
   if (event_flags_ == EventHandler::NONE) {
       io_handler_.UnregisterHandler();
       return ;
   }
   io_handler_.RegisterHandler(event_flags_);
}

void AsyncSocket::UpdateEventRegistration(uint16_t enable, uint16_t disable) {
    event_flags_ |= enable;
    event_flags_ &= ~disable;
    UpdateEventRegistration();
}

