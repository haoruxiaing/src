#ifndef NET_ASYNC_SOCKET_H
#define NET_ASYNC_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <glog/logging.h>

#include "buffer.h"
#include "event_handler.h"
#include "socket_address.h"
#include "async_timeout.h"

class EventLoop;

// 支持异步I/O的socket封装
class AsyncSocket {
public:
    class ConnectCallback {
    public:
        virtual ~ConnectCallback() = default;
 
        virtual void ConnectSuccess() = 0; 
        virtual void ConnectErr() = 0;
    };

    class ReadCallback {
    public:
        virtual ~ReadCallback() = default;
        virtual void ReadDataAvailable(Buffer* message) = 0;
        virtual void ReadErr(int err) = 0;
        virtual void ReadEOF() = 0;
        virtual void ReadTimeout() = 0;
    };

    AsyncSocket() = delete;
    explicit AsyncSocket(EventLoop* evl);

    AsyncSocket(EventLoop* evl, int fd);

    AsyncSocket(EventLoop* evl, 
              const SocketAddress& address,
              uint32_t connect_timeout);
  
    AsyncSocket(EventLoop* evl,
              const std::string& ip,
              uint16_t port,
              uint32_t connect_timeout);

    ~AsyncSocket();

    EventLoop* GetEventLoop() const { 
        return evl_;
    }

    int GetFd() const {
        return fd_;
    }

    void Connect(ConnectCallback* callback,
               const SocketAddress& address,
               uint32_t timeout = 0);
    void Connect(ConnectCallback* callback,
               const std::string& ip,
               uint16_t port,
               uint32_t timeout = 0);

    void CancelConnect();

    void SetConnectCB(ConnectCallback* callback) {
        connect_callback_ = callback;
    }

    void SetReadCB(ReadCallback* callback);
    ReadCallback* GetReadCB() const { return read_callback_; }

    void Write(const char* buf, size_t len);
    void Write(const std::string& message);
  
    void Close();
    void CloseNow();
    void ShutdownWrite() { } 
    void ShutdownWriteNow() { }

    void GetLocalAddress(SocketAddress* address);
    void GetPeerAddress(SocketAddress* address);

    bool connecting() {
        return state_ == StateEnum::CONNECTING; 
    }

    bool connected() {
        return state_ == StateEnum::ESTABLISHED;
    }

    int SetNoDelay(bool nodelay);
    void SetCloseOnExec();
    int SetSendBufSize(size_t bufsize);
    int SetRecvBufSize(size_t bufsize);

    void ScheduleReadTimeout(uint32_t milliseconds) {
        readtimeout_.ScheduleTimeout(milliseconds);
    }
  
    void CancelReadTimeout() {
        readtimeout_.CancelTimeout();
    }
  
    enum StateEnum {
        UNINIT,
        CONNECTING,
        ESTABLISHED,
        CLOSED,
        ERROR
    };
private:
  // 读超时，在连接时或连接成功后等待消息时使用
    class ReadTimeout : public AsyncTimeout {
    public:
        ReadTimeout(AsyncSocket* socket, EventLoop* evl)
           : AsyncTimeout(evl) 
           , socket_(socket) {
        }

        virtual void TimeoutExpired() {
            socket_->TimeoutExpired();
        }
    private:
        AsyncSocket* socket_;
    };

    class IoHandler : public EventHandler {
    public:
        IoHandler(AsyncSocket* socket, EventLoop* evl, int fd)
            : EventHandler(evl, fd)
            , socket_(socket) {
        }
    
        virtual void HandlerReady(uint16_t events) {
            socket_->IoReady(events);
        }
    private:
        AsyncSocket* socket_;
    };

    enum ShutdownFlags {
        SHUT_WRITE_PENDING = 0x01,
        SHUT_WRITE = 0x02,
        SHUT_READ = 0x04
    };
    
    void Init();

    void IoReady(uint16_t events);
    void HandleRead();
    void HandleWrite();
    void HandleConnect();
    void HandleInitialReadWrite();
    void TimeoutExpired();

    ssize_t PerformRead();
    void PerformWrite(const std::string& message);

    void UpdateEventRegistration();
    void UpdateEventRegistration(uint16_t enable, uint16_t disable);

    StateEnum state_;             // 当前的连接状态
    uint8_t shutdown_flags_;      // socket关闭标识
    uint16_t event_flags_;        // EventHandler::HandleFlags 
    int fd_;                      // socket
    SocketAddress addr_;          // 连接的目的地址
    uint32_t read_timeout_;       // 读超时(秒)

    EventLoop* evl_;
    ReadTimeout readtimeout_;     // 连接/读超时
    IoHandler io_handler_;        // I/O监视器

    ConnectCallback* connect_callback_;
    ReadCallback* read_callback_;

    Buffer input_buffer_;         // 读缓冲区
    Buffer output_buffer_;        // 写缓存区
};

#endif

