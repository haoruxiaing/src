#include "socket_address.h"

#include <netdb.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include <glog/logging.h>

const SA* sockaddr_cast(const struct sockaddr_in* addr) {
    return static_cast<const SA*>(static_cast<const void*>(addr));
}

SA* sockaddr_cast(struct sockaddr_in* addr) {
    return static_cast<SA*>(static_cast<void*>(addr));
}

// INADDR_ANY use (type)value casting.
#pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;
#pragma GCC diagnostic error "-Wold-style-cast"

//     /* Structure describing an Internet socket address.  */
//     struct sockaddr_in {
//         sa_family_t    sin_family; /* address family: AF_INET */
//         uint16_t       sin_port;   /* port in network byte order */
//         struct in_addr sin_addr;   /* internet address */
//     };

//     /* Internet address. */
//     typedef uint32_t in_addr_t;
//     struct in_addr {
//         in_addr_t       s_addr;     /* address in network byte order */
//     };

SocketAddress::SocketAddress(uint16_t port, bool loopback_only) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopback_only ? kInaddrLoopback : kInaddrAny;
    addr_.sin_addr.s_addr = htonl(ip);
    addr_.sin_port = htons(port);
}

SocketAddress::SocketAddress(const std::string& ip, uint16_t port) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    addr_.sin_port = htons(port);
}

SocketAddress::SocketAddress(const struct sockaddr_in& addr) : addr_(addr) { }

void SocketAddress::SetFromLocalAddress(int fd) {
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (::getsockname(fd, sockaddr_cast(&localaddr), &addrlen) < 0) {
        LOG(ERROR) << "failed to called getsockname(), errno=" << errno;
        return ;
    }
    addr_ = localaddr;
}

void SocketAddress::SetFromPeerAddress(int fd) {
    struct sockaddr_in peeraddr;
    bzero(&peeraddr, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (::getpeername(fd, sockaddr_cast(&peeraddr), &addrlen) < 0) {
        LOG(ERROR) << "failed to called getpeername(), errno=" << errno;
        return ;
    }
    addr_ = peeraddr;
}

std::string SocketAddress::ToIp() const {
    char buf[32];
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, static_cast<socklen_t>(sizeof(buf)));
    return buf;
}

std::string SocketAddress::ToIpPort() const {
    char buf[32];
    size_t size = sizeof(buf);
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, static_cast<socklen_t>(size));
    size_t end = ::strlen(buf);
    uint16_t port = ::ntohs(addr_.sin_port);
    ::snprintf(buf+end, size-end, ":%u", port);
    return buf;
}

static __thread char g_resolve_buffer[64*1024];
bool SocketAddress::Resolve(std::string&& hostname, SocketAddress* result) {
    struct hostent hent;
    struct hostent* he = NULL;
    int herrno = 0;
    bzero(&hent, sizeof(hent));

    int ret = gethostbyname_r(hostname.c_str(), &hent, g_resolve_buffer,
                            sizeof(g_resolve_buffer), &he, &herrno);
    if (ret == 0 && he != NULL) {
        result->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        return true;
    }
    return false;
}

