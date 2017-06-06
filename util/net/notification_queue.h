#ifndef NET_NOTIFICATION_QUEUE_H
#define NET_NOTIFICATION_QUEUE_H

#include <fcntl.h>
#include <unistd.h>
#include <sys/eventfd.h>

#include <deque>
#include <glog/logging.h>

#include "event_loop.h"
#include "event_handler.h"

// 一个通过EventLoop线程传递消息的生产者-消费者队列
template<typename MessageT>

class NotificationQueue
{
public:
  // 回调接口定义类,当队列有消息需要消费时调用
    class Consumer : private EventHandler {
    public:
        Consumer() : queue_(0) {}
        virtual ~Consumer() {}
        virtual void MessageAvailable(MessageT&& message) = 0;
        void StartConsuming(EventLoop* event_loop, NotificationQueue* queue) {
            Init(event_loop, queue);
            RegisterHandler(EventFlags::READ);
        }

        void StopConsuming() {
            if (queue_ == 0) return ;
            UnregisterHandler();
            queue_ = 0;
        }

        NotificationQueue* GetCurrentQueue() const {
            return queue_;
        }

        virtual void HandlerReady(uint16_t events);
    private:
        void ConsumeMessages();
        void Init(EventLoop* event_loop, NotificationQueue* queue);

        NotificationQueue* queue_;
        EventLoop* loop_;
    };

    NotificationQueue(uint32_t max_size = 0) 
        : eventfd_(-1)
        , max_queue_size_(max_size)
        , queue_()
    {
        eventfd_ = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK | EFD_SEMAPHORE);
        if (eventfd_ == -1) {
            if (errno == ENOSYS || errno == EINVAL) {
                LOG(ERROR) << "failed to create eventfd for NotificationQueue: "
                         << errno << ", is your kernel > 2.6.30?)";
            } else {
              LOG(ERROR) << "failed to create eventfd for NotificationQueue: "
                         << errno << ", aborting";
            }
            ::abort();
        }
    }

    ~NotificationQueue() {
        if (eventfd_ >= 0) {
            ::close(eventfd_);
            eventfd_ = -1;
        }
    }

    void SetMaxQueueSize(uint32_t max) {
        max_queue_size_ = max;
    }

    void PutMessage(MessageT&& message) 
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push_back(std::move(message));
        }
        SignalEvent();
    }

    bool TryPutMessage(MessageT&& message) 
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!CheckQueueSize()) {
                return false;
            }
            queue_.push_back(std::move(message));
        }
        SignalEvent();
        return true;
    }

    bool Consume(MessageT& result) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }

        result = queue_.front();
        queue_.pop_front();
        return true;
    }
private:
    NotificationQueue(NotificationQueue const &) = delete;
    NotificationQueue& operator=(NotificationQueue const &) = delete;

    inline bool CheckQueueSize() {
        if (max_queue_size_ > 0 && queue_.size() >= max_queue_size_) {
            return false;
        }
        return true;
    }

    inline void SignalEvent() {
        if (eventfd_ >= 0) {
            uint64_t numadded64 = 1;
            ssize_t bytes_written = ::write(eventfd_, &numadded64, 
                                          sizeof(numadded64));
            if (bytes_written != sizeof(numadded64)) {
                LOG(ERROR) << "failed to signal NotificationQueue after write,"
                         << " errno=" << errno;
            }
        }
    }

    void ConsumeEvent() {
        uint64_t value = 0;
        ssize_t rc = 0;
        if (eventfd_ >= 0) {
            rc = ::read(eventfd_, &value, sizeof(value));
        }
        (void)rc;
    }

    int eventfd_;
    uint32_t max_queue_size_;
    std::deque<MessageT> queue_;
    std::mutex mutex_;
};

template<typename MessageT>
void NotificationQueue<MessageT>::Consumer::HandlerReady(uint16_t events){
    queue_->ConsumeEvent();
    ConsumeMessages();
}

template<typename MessageT> 
void NotificationQueue<MessageT>::Consumer::ConsumeMessages(){
    MessageT message;
    {
        std::lock_guard<std::mutex> lock(queue_->mutex_);
        if (queue_->queue_.empty()) {
            return ;
        }

        message = queue_->queue_.front();
        queue_->queue_.pop_front();
    }
    MessageAvailable(std::move(message));
}

template<typename MessageT>
void NotificationQueue<MessageT>::Consumer::Init(EventLoop* event_loop,
                                                 NotificationQueue* queue) {
    loop_ = event_loop;
    queue_ = queue;
    CHECK_GE(queue_->eventfd_, 0); 
    InitHandler(event_loop, queue_->eventfd_);
}

#endif

