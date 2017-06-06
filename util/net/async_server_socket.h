#ifndef NET_ASYNC_SERVER_SOCKET_H
#define NET_ASYNC_SERVER_SOCKET_H

#include "event_handler.h"
#include "event_loop.h"
#include "async_timeout.h"
#include "socket_address.h"

#include <vector>
#include <limits.h>
#include <stddef.h>
#include <sys/socket.h>

class AsyncServerSocket {
public:
    static const uint32_t kDefaultMaxAcceptAtOnce = 30;
  
    class AcceptCallback {
    public:
        virtual ~AcceptCallback() {}

        virtual void ConnectionAccepted(int fd, const SocketAddress& client_addr) = 0;
    };

    AsyncServerSocket(EventLoop* evl);
    ~AsyncServerSocket();

    EventLoop* GetEventLoop() const { 
        return evl_;
    }

    bool Bind(const SocketAddress& address);

    // 绑定指定的地址，非线程安全
    bool Bind(uint16_t port);

    // 获取绑定的本地地址
    void GetAddress(SocketAddress* address);

    bool Listen();

    void StartAccepting();
    void StopAccepting();

    void SetAcceptCallback(AcceptCallback* callback);
    void RemoveAcceptCallback();

    void SetMaxAcceptAtOnce(uint32_t num_conns) {
        max_accept_at_once_ = num_conns;
    }
private:
    void HandlerReady(uint16_t events, int fd) ;

    int CreateSocket();
    int SetupSocket(int fd);
    int BindSocket(int fd, const SocketAddress& address);

    struct ServerEventHandler : public EventHandler {
        ServerEventHandler(EventLoop* evl, 
                         int fd,
                         AsyncServerSocket* parent) 
          : EventHandler(evl, fd)
          , evl_(evl)
          , socket_(fd)
          , parent_(parent) {}

        void InitHandler(EventLoop* loop, int fd) {
            evl_ = loop;
            socket_ = fd;
            EventHandler::InitHandler(loop, fd);
        }

        virtual void HandlerReady(uint16_t events) {
            parent_->HandlerReady(events, socket_);
        }

        EventLoop* evl_;
        int socket_;
        AsyncServerSocket* parent_;
    }; 

    EventLoop* evl_;
    ServerEventHandler server_handler_;
    uint32_t max_accept_at_once_;
    bool accepting_;
    AcceptCallback* accept_callback_;
};

#endif

