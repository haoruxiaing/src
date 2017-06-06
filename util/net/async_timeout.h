#ifndef NET_ASYNC_TIMEOUT_H
#define NET_ASYNC_TIMEOUT_H

#include <stdint.h>
#include <chrono>
#include "libev/ev.h"

#include "timeout_manager.h"

class EventLoop;
class AsyncTimeout {
public:
    explicit AsyncTimeout(TimeoutManager* timeout_manager);
    explicit AsyncTimeout(EventLoop* loop); 
    virtual ~AsyncTimeout();
    virtual void TimeoutExpired() = 0;
    void ScheduleTimeout(uint32_t milliseconds);
    void ScheduleTimeout(TimeoutManager::timeout_type timeout);
    void CancelTimeout();
    bool IsSchedule() const {
        return ev_is_pending(&ev_) || ev_is_active(&ev_);
    }
    TimeoutManager* GetTimeoutManager() const {
        return timeout_manager_;
    }
    struct ev_timer* GetEvent(){
        return &ev_;
    }
private:
    static void LibevCallback(struct ev_loop* loop, struct ev_timer* ev, int events);
    struct ev_timer ev_;
    TimeoutManager* timeout_manager_;
};

#endif

