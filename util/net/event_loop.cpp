#include "event_loop.h"
#include <sys/eventfd.h>
#include <cstdlib>
#include "event_handler.h"

EventLoop::EventLoop(struct ev_loop* loop)
    : stop_(false)
    , loop_(loop)
    , queue_(new NotificationQueue<Callback>())
    , fn_runner_(new FunctionRunner()) {
    if (!loop_) {
        VLOG(5) << "loop is nullptr, create new one.";
        loop_ = ev_loop_new();
        if (!loop_) {
            LOG(ERROR) << "create ev_loop failure, abort...";
            std::abort();
        }
    }
    VLOG(5) << "EventLoop[" << this << "] ctor";
}

EventLoop::~EventLoop() {
    stop_ = true;
    
    ev_loop_destroy(loop_);
    VLOG(5) << "EventLoop[" << this << "] dtor";
}

bool EventLoop::Loop() {
    VLOG(5) << "EventLoop(): Starting loop.";
    //loop_thread_.store(pthread_self(), std::memory_order_release);

    fn_runner_->StopConsuming();
    fn_runner_->StartConsuming(this, queue_.get());

    while (!stop_) {
        ev_run(loop_, 0);
    }

    // 重置stop_，以便可以再次调用Loop()。
    stop_ = false;
    //loop_thread_.store(0, std::memory_order_release);

    fn_runner_->StopConsuming();
    VLOG(5) << "EventLoop(): Done with loop.";
    return true;
}

void EventLoop::TerminateLoopSoon() {
    VLOG(5) << "EventLoop(): Recevicee TerminateLoopSoon command.";
    stop_ = true;

    // 线程可能在等待事件就绪，需要主动唤醒线程。
    queue_->PutMessage(std::bind(&EventLoop::BreakLoop, this));
}

void EventLoop::RunInEventLoopThread(Callback&& cb) {
    queue_->PutMessage(std::move(cb));
}

void EventLoop::ScheduleTimeout(AsyncTimeout* obj, timeout_type timeout) {
    assert(IsInEventLoopThread());
    struct ev_timer* ev = obj->GetEvent();
    ev_timer_stop(loop_, ev);
    ev_timer_set(ev, static_cast<double>(timeout.count()/1000.0f), 0.); 
    ev_timer_start(loop_, ev);
}

void EventLoop::CancelTimeout(AsyncTimeout* obj) {
    assert(IsInEventLoopThread());
    struct ev_timer* ev = obj->GetEvent();
    ev_timer_stop(loop_, ev);
}
