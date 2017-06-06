#include "tcpserver.h"

#include <functional>

TcpServer::TcpServer(const SocketAddress& bind_address,
                     std::vector<void*> contexts, 
                     uint32_t num_threads)
    : main_loop_(new EventLoop()) 
    , socket_(new AsyncServerSocket(main_loop_))
    , bind_address_(bind_address)
    , main_thread_(0) 
    , max_accept_at_once_(0)
    //, next_io_thread_(0)
    , contexts_(contexts)
    , num_threads_(num_threads)
    , callback_(0)
    {assert(num_threads == contexts.size());}

TcpServer::~TcpServer() {
    for (unsigned int i=0; i<threads_.size(); i++) {
        delete threads_[i];
    }
    threads_.clear();

    if (main_thread_) {
        main_thread_->join();
        delete main_thread_;
    }
    
    delete main_loop_;
    delete socket_;
}

void TcpServer::Start() {
    assert(main_thread_ == 0);

    if (main_thread_ == 0) {
        main_thread_ = new std::thread(&TcpServer::MainLoopThread, this);

        if (num_threads_ > 0) {
            for (int i = 0; i < num_threads_; ++i) {
                IOThread* io_thread = new IOThread();
                io_thread->SetContext(contexts_[i]);
                threads_.push_back(io_thread);
            }
        }
    }
}

void TcpServer::Stop() {
    assert(main_thread_ != 0);

    if (main_loop_->IsInEventLoopThread()) {
        socket_->StopAccepting();
    } else {
        main_loop_->RunInEventLoopThread(
            std::bind(&TcpServer::StopInLoop, this));
    }
}

void TcpServer::SetThreadCount(uint32_t num_threads) {
    num_threads_ = num_threads;
}

void TcpServer::SetTcpCallback(TcpConnection::Callback* callback) {
    callback_ = callback;
}

void TcpServer::SetMaxAcceptAtOnce(uint32_t num_conns) {
    max_accept_at_once_ = num_conns;
}

TcpConnection* TcpServer::GetConnection(int fd) {
    std::lock_guard<std::mutex> lock(mutex_);
    TcpConnection* conn = 0;
    ConnectionMap::const_iterator it = connections_.find(fd);
    if (it != connections_.end()) {
        conn = it->second;
    }
    return conn;
}

void TcpServer::RemoveConnection(const TcpConnection* conn) {
    RemoveConnection(conn->GetFd());
}

void TcpServer::RemoveConnection(int fd) {
    TcpConnection* conn = 0;
    EventLoop* loop = 0;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = connections_.find(fd);
        if (it != connections_.end()) {
            conn = it->second;
            loop = conn->GetEventLoop();
            VLOG(12) << "remove connection " << conn;
        }
        connections_.erase(fd);
    }
    if (loop) {
        /*
        loop->RunInEventLoopThread([conn]() {
            if (conn) {
                delete conn;
            }
        });*/
    }
}

void TcpServer::ConnectionAccepted(int fd,
                                   const SocketAddress& client_addr) {
    TcpConnection* new_conn = 0;
    if (num_threads_ == 0) {
        new_conn = new TcpConnection(main_loop_, fd);
        new_conn->SetCallback(callback_);
        new_conn->ConnectEstablished();
    } else {
       //IOThread* io_thread = NextIOThread();
       IOThread* io_thread = GetIOThread(fd);
       new_conn = new TcpConnection(io_thread->GetEventLoop(), fd);
       new_conn->SetCallback(callback_);
       QueueMessage msg;
       msg.type = MessageType::NEW_CONN_TYPE;
       msg.conn = new_conn;
       io_thread->GetQueue()->PutMessage(std::move(msg));
    }

    std::lock_guard<std::mutex> lock(mutex_);
    connections_[fd] = new_conn;
}

void TcpServer::MainLoopThread() {   
    if (!socket_->Bind(bind_address_)) {
        LOG(ERROR) << "failed to bind " << bind_address_.ToIpPort() 
                   << ", aborting";
        ::abort();
    }

    if (!socket_->Listen()) {
        LOG(ERROR) << "failed to listen, aborting";
        ::abort();
    }
    if (max_accept_at_once_ > 0) {
      socket_->SetMaxAcceptAtOnce(max_accept_at_once_);
    }
    socket_->SetAcceptCallback(this);
    socket_->StartAccepting();

    main_loop_->Loop();
}

void TcpServer::StopInLoop() {
    assert(main_loop_->IsInEventLoopThread());

    socket_->StopAccepting();
    main_loop_->TerminateLoopSoon();
}

