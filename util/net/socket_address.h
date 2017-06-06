#ifndef NET_SOCKET_ADDRESS_H
#define NET_SOCKET_ADDRESS_H

#include <netinet/in.h>

#include <string>

typedef struct sockaddr SA;

const SA* sockaddr_cast(const struct sockaddr_in* addr);
SA* sockaddr_cast(struct sockaddr_in* addr);

// sockaddr_in封装
class SocketAddress {
 public:
  explicit SocketAddress(uint16_t port = 0, bool loopback_only = false);
  SocketAddress(const std::string& ip, uint16_t port);
  SocketAddress(const struct sockaddr_in& addr);
  ~SocketAddress() = default;

  void SetFromLocalAddress(int fd);
  void SetFromPeerAddress(int fd);

  std::string ToIp() const;
  std::string ToIpPort() const;

  const struct sockaddr_in& addr() const { return addr_; }
  void set_addr(const struct sockaddr_in& addr) { addr_ = addr; }

  // 根据hostname获取IP地址
  static bool Resolve(std::string&& hostname, SocketAddress* result);
 private:
  struct sockaddr_in addr_;
};

#endif

