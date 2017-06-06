#ifndef NET_TCPCONNECTION_H
#define NET_TCPCONNECTION_H

#include <memory>

#include "async_socket.h"
#include "event_loop.h"

// TCP连接封装
class TcpConnection : public AsyncSocket::ReadCallback {
public:
    // 回调接口定义
    class Callback {
    public:
        virtual ~Callback() {}

        virtual void OnConnect(TcpConnection* conn)  = 0;
        virtual void OnClose(TcpConnection* conn)  = 0;
        virtual void OnMessage(TcpConnection* conn, Buffer* message)  = 0;
        virtual void OnTimeout(TcpConnection* conn)  = 0;
    };

    explicit TcpConnection(EventLoop* loop, int fd);
    ~TcpConnection();
  
    void Close();
    void CloseInLoop();

    void Send(const char* data, size_t len);
    void Send(const std::string& message);

    void ConnectEstablished();

    int GetFd() const { 
        return socket_->GetFd();
    }

    EventLoop* GetEventLoop() {
        return loop_;
    }

    bool connected() {
        return socket_ ? socket_->connected() : false;
    }  

    void GetLocalAddress(SocketAddress* address);
    void GetPeerAddress(SocketAddress* address);

    void SetCallback(Callback* callback);
    void SetUserData(void* data) { userdata_ = data; }
    void* GetUserData() { return userdata_; }

    void ScheduleReadTimeout(uint32_t timeout);
    void CancelReadTimeout();
private:
    virtual void ReadDataAvailable(Buffer* message);
    virtual void ReadErr(int err);
    virtual void ReadEOF();
    virtual void ReadTimeout();

    void SendInLoop(const std::string& message);

    EventLoop* loop_;
    int fd_;
    //std::shared_ptr<AsyncSocket> socket_;
    AsyncSocket* socket_;
    Callback* callback_;
    void* userdata_;
    SocketAddress local_address_;
    SocketAddress peer_address_;
};

#endif
