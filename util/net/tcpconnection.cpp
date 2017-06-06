#include "tcpconnection.h"

TcpConnection::TcpConnection(EventLoop* loop, int fd) 
    : loop_(loop)
    , fd_(fd)
    , socket_(0)
    , callback_(0)
    , userdata_(0) {
    local_address_.SetFromLocalAddress(fd);
    peer_address_.SetFromPeerAddress(fd);
}

TcpConnection::~TcpConnection() {
  /*
    if (socket_ && socket_->connected()) {
        socket_->SetReadCB(nullptr);
        Close();
    }
    */
    if (socket_) delete socket_;
}

void TcpConnection::Close() {
    if (socket_) {
        if (loop_->IsInEventLoopThread()) {
            socket_->Close();
        } else {
            loop_->RunInEventLoopThread(
                std::bind(&AsyncSocket::Close, socket_));    
        }
    }
}

void TcpConnection::CloseInLoop() {
    if (socket_) {
         loop_->RunInEventLoopThread(
               std::bind(&AsyncSocket::Close, socket_)
         );    
    }
}

void TcpConnection::Send(const char* data, size_t len) {
    Send(std::string(data, len));
}

void TcpConnection::Send(const std::string& message) {
    if (!socket_ || !socket_->connected()) {
        LOG(ERROR) << "the connection closed, discard this message";
        return;
    } 

    if (loop_->IsInEventLoopThread()) {
        socket_->Write(message);
    } else {
        loop_->RunInEventLoopThread(
            std::bind(&TcpConnection::SendInLoop, this, message));
    }
}

void TcpConnection::ConnectEstablished() {
    assert(loop_->IsInEventLoopThread());

    //socket_.reset(new AsyncSocket(loop_, fd_));
    socket_ = new AsyncSocket(loop_, fd_);
    socket_->SetReadCB(this);

    if (callback_) {
        callback_->OnConnect(this);
    } else {
        LOG(WARNING) << "callback_ is not set, the connect is unused";
    }
}

void TcpConnection::GetLocalAddress(SocketAddress* address) {
    *address = local_address_;
}

void TcpConnection::GetPeerAddress(SocketAddress* address) {
    *address = peer_address_;
}

void TcpConnection::SetCallback(Callback* callback) {
    callback_ = callback;
}

void TcpConnection::ScheduleReadTimeout(uint32_t timeout) {
    if (timeout > 0 ) {
        socket_->ScheduleReadTimeout(timeout);
    }
}

void TcpConnection::CancelReadTimeout() {
    socket_->CancelReadTimeout();
}

void TcpConnection::ReadDataAvailable(Buffer* message){
    if (callback_) {
        callback_->OnMessage(this, message);
    } else {
        LOG(WARNING) << "callback_ is not set, discard this message";
        message->RetrieveAll();
    }
}

void TcpConnection::ReadErr(int err){
    if (callback_) {
        callback_->OnClose(this);
    }
    socket_->CancelReadTimeout();
    socket_->Close();
    //socket_.reset();
}

void TcpConnection::ReadEOF(){
    if (callback_) {
        callback_->OnClose(this);
    }
    socket_->CancelReadTimeout();
    socket_->Close();
    //socket_.reset();
}

void TcpConnection::ReadTimeout(){
    if (callback_) {
        callback_->OnTimeout(this);
    }
}

void TcpConnection::SendInLoop(const std::string& message) {
    assert(loop_->IsInEventLoopThread());
    
    socket_->Write(message);
}


