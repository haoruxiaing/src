#ifndef NET_EVENT_HANDLER_H
#define NET_EVENT_HANDLER_H

#include <stddef.h>
#include "libev/ev.h"
#include <glog/logging.h>

class EventLoop;

class EventHandler {
public:
    enum EventFlags {
        NONE = EV_NONE,
        READ = EV_READ,
        WRITE = EV_WRITE,
        READ_WRITE = (EV_READ | EV_WRITE)
    };

    EventHandler();
    explicit EventHandler(EventLoop* loop, int fd);
    virtual ~EventHandler();

    void InitHandler(EventLoop* loop, int fd);

    // 事件就绪时会调用此函数
    virtual void HandlerReady(uint16_t events) = 0;

    void RegisterHandler(int events);
    void UnregisterHandler();

    void ChangeHandlerFD(int fd);
private:
    static void LibevCallback(struct ev_loop* loop, struct ev_io* ev, int events);

    struct ev_io ev_;
    EventLoop* loop_;
};

#endif
