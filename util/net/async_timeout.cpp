#include "async_timeout.h"
#include "event_loop.h"

AsyncTimeout::AsyncTimeout(TimeoutManager* timeout_manager)
    : timeout_manager_(timeout_manager) {
    ev_timer_init(&ev_, &AsyncTimeout::LibevCallback, 0., 0.);
    ev_.data = this;
}

AsyncTimeout::AsyncTimeout(EventLoop* loop) 
    : timeout_manager_(loop) {
    ev_timer_init(&ev_, &AsyncTimeout::LibevCallback, 0., 0.);
    ev_.data = this;
}

AsyncTimeout::~AsyncTimeout() {
    CancelTimeout();
}

void AsyncTimeout::ScheduleTimeout(uint32_t milliseconds) {
    ScheduleTimeout(TimeoutManager::timeout_type(milliseconds));
}

void AsyncTimeout::ScheduleTimeout(TimeoutManager::timeout_type timeout) {
    assert(timeout_manager_ != 0);
    timeout_manager_->ScheduleTimeout(this, timeout);
}

void AsyncTimeout::CancelTimeout() {
    if (IsSchedule()) {
        timeout_manager_->CancelTimeout(this);
    }
}

void AsyncTimeout::LibevCallback(struct ev_loop* loop,
                                 struct ev_timer* ev,
                                 int events) {
    AsyncTimeout* obj = reinterpret_cast<AsyncTimeout*>(ev->data);
    obj->TimeoutExpired();
}

