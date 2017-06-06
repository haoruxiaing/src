#ifndef NET_TCPSERVER_H
#define NET_TCPSERVER_H

#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <thread>

#include "async_server_socket.h"
#include "notification_queue.h"
#include "tcpconnection.h"


// TCP服务器封装
class TcpServer : public AsyncServerSocket::AcceptCallback
{
public:
    TcpServer(const SocketAddress& bind_address,
            std::vector<void*> contexts, 
            uint32_t num_threads = 0);
    virtual ~TcpServer();

    void Start();
    void Stop();

    // 设置线程数量
    void SetThreadCount(uint32_t num_threads);

    // 设置TCP连接回调
    void SetTcpCallback(TcpConnection::Callback* callback);

    // 设置一次事件accept的连接数量
    void SetMaxAcceptAtOnce(uint32_t num_conns); 

    TcpConnection* GetConnection(int fd);
    void RemoveConnection(const TcpConnection* conn);
    void RemoveConnection(int fd);

    virtual void ConnectionAccepted(int fd, const SocketAddress& client_addr);
private:
    enum MessageType {
        NEW_CONN_TYPE = 0,
        CONN_CLOSED = 1
    };

    struct QueueMessage {
        MessageType type;
        TcpConnection* conn;
    };

    class IOThread : public NotificationQueue<QueueMessage>::Consumer {
    public:
        IOThread() 
            : queue_()
            , loop_()
            , context_(NULL)
            , thread_(&IOThread::ThreadLoop, this) {}

        virtual ~IOThread() {
            StopConsuming();
            loop_.TerminateLoopSoon();
            thread_.join();
        }

        EventLoop* GetEventLoop() { 
            return &loop_;
        }

        NotificationQueue<QueueMessage>* GetQueue() {
            return &queue_;
        }

        virtual void MessageAvailable(QueueMessage&& message) {
            if (message.type == MessageType::NEW_CONN_TYPE && message.conn) {
                message.conn->ConnectEstablished(); 
            } else if (message.type == MessageType::CONN_CLOSED) {
                // ignore
            } else {
                // ignore
            }
        }
        void* GetContext() { 
            return context_; 
        }

        void SetContext(void* ctx) { 
            context_ = ctx; 
        }

    private:
        void ThreadLoop() {
            sigset_t ss;
            sigemptyset(&ss);
            sigaddset(&ss, SIGHUP);
            sigaddset(&ss, SIGINT);
            sigaddset(&ss, SIGQUIT);
            sigaddset(&ss, SIGUSR1);
            sigaddset(&ss, SIGUSR2);
            sigaddset(&ss, SIGPIPE);
            sigaddset(&ss, SIGALRM);
            sigaddset(&ss, SIGTERM);
            sigaddset(&ss, SIGCHLD);
            sigaddset(&ss, SIGIO);
            PCHECK(pthread_sigmask(SIG_BLOCK, &ss, 0) == 0);
            StartConsuming(&loop_, &queue_);
            loop_.Loop();
        }
        NotificationQueue<QueueMessage> queue_;
        EventLoop loop_;
        void* context_;
        std::thread thread_;
    };
public:
    void* GetIOThreadContext(int fd)
    {
        IOThread* io_thread = GetIOThread(fd);
        assert(io_thread != 0);
        return io_thread->GetContext();
    }

private:
    void MainLoopThread();
    void StopInLoop();

    //IOThread* NextIOThread() {
    IOThread* GetIOThread(int fd) {
#if 0
      IOThread* io_thread = threads_[next_io_thread_];

      ++next_io_thread_;
      if (next_io_thread_ >= threads_.size()) {
          next_io_thread_ = 0;
      }
#else
      IOThread* io_thread = threads_[fd%threads_.size()];
#endif
        return io_thread;
    }
    typedef std::map<int, TcpConnection*> ConnectionMap;
    EventLoop* main_loop_;
    AsyncServerSocket* socket_;
    SocketAddress bind_address_;
    std::thread* main_thread_;
    std::vector<IOThread*> threads_;
    uint32_t max_accept_at_once_;
    //uint32_t next_io_thread_;
    std::vector<void*> contexts_;
    int num_threads_;
    std::mutex mutex_;
    ConnectionMap connections_;  // guard by mutex_
    TcpConnection::Callback* callback_;
};


#endif
