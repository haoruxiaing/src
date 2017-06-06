#ifndef NET_EVENT_LOOP_H
#define NET_EVENT_LOOP_H

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <functional>
//#include <atomic>
#include <deque>
#include <mutex>
#include <memory>

#include "libev/ev.h"

#include "event_handler.h"
#include "async_timeout.h"
#include "timeout_manager.h"
#include "notification_queue.h"

// EventLoop是对libev的ev_loop的封装。
// 
// EventLoop提供一个主循环，当I/O就绪时通知EventHandler的回调函数,或定时器超时时
// 回调AsyncTimeout的回调函数。提供回调函数的类必须是EventHandler或AsyncTimeout
// 的派生类。
//
// 一个线程只能持有一个EventLoop对象，并且这个EventLoop只能驱动一个ev_loop。在多
// 线程环境下，应该为每个线程分配一个EventLoop对象。
//
// EventLoop的函数一般情况下只能在运行它的线程内调用，其它线程要与当前线程通讯，
// 必须通过EventLoop；EventLoop内部维护一个list，所有其他线程的调用都在这个list
// 内排队调用。
class EventLoop : public TimeoutManager {
public:
    typedef std::function<void()> Callback;

    // 通过ev_loop创建一个EventLoop对象，ev_loop后续由EventLoop持有，并在EventLoop
    // 析构时调用ev_loop_destroy()释放。
    explicit EventLoop(struct ev_loop *loop = 0);
    virtual ~EventLoop();

    // 事件循环
    //
    // Loop()会阻塞等待I/O或定时器超时，当事件就绪时回调EventHandler或AsyncTimeout
    // 的回调函数。Loop()在没有事件需要执行或在TerminateLoopSoon()被调用时
    //
    // 在Loop()返回后，Loop()可以重复调用去重启事件循。
    //
    // 事件循环过程，若无任何错误返回true，否则返回false。
    bool Loop();

    // 通知EventLoop退出
    //
    // 这个调用将会导致Loop()停止事件循环并退出，即使还有事件等待处理。
    //
    // TerminateLoopSoon()是线程安全，可以在其他线程调用，会唤醒EventLoop的线程。
    // TerminateLoopSoon()也可以在EventLoop的线程内调用（如：EventHandler或Async-
    // Timeout会在回调内调用TerminateLoopSoon()，在回调返回时结束事件循环。）
    // 如果事件循环没有启动，那么在下个事件循环启动后会导致马上退出循环。
    // 
    // 调用者需要注意的是，TiminateLoopSoon()会导致loop退出，阻塞的事件无法继续
    // 处理。当然再次进入事件循环可以继续处理阻塞的事件。
    void TerminateLoopSoon();

    // 在EventLoop的线程内运行指定的函数
    //
    // 这个函数是线程安全的，可以在任意线程调用。
    // 
    // 在loop未启动时，调用RunInEventLoopThread()会延迟到loop下次启动时才有效。
    void RunInEventLoopThread(Callback&& cb);

    // 检查EventLoop是否运行。
    bool IsRunning() const {
        return 1;
        //return loop_thread_.load(std::memory_order_relaxed) != 0; 
    }

    // 如果EventLoop已运行，检查当前的线程是不是EventLoop的运行线程
    bool IsInEventLoopThread() const {
        //auto tid = loop_thread_.load(std::memory_order_relaxed);
        //return tid == 0 || pthread_equal(tid, pthread_self());
	return 1;
    }

    struct ev_loop* GetLibevLoop() const { return loop_; }
private:
    class FunctionRunner : public NotificationQueue<Callback>::Consumer {
    public:
        FunctionRunner() = default;
        virtual ~FunctionRunner() = default;

        virtual void MessageAvailable(Callback&& cb) {
            if (cb) {
                cb();
            } else {
                LOG(ERROR) << "error cb";
            }
        }
    };

    void BreakLoop() {
        ev_break(loop_);
    }

    // 调度定时器 
    //
    // @param timeout 超时时间(毫秒)
    // 
    virtual void ScheduleTimeout(AsyncTimeout* obj, timeout_type timeout);

    virtual void CancelTimeout(AsyncTimeout* obj);

    virtual bool IsInTimeoutManagerThread() { return IsInEventLoopThread(); }

    // 停止标识位，由TerminateLoopSoon()设置。
    bool stop_;

    // 运行EventLoop的线程ID。
    // ID为0表示EventLoop未运行。
    //std::atomic<pthread_t> loop_thread_;

    struct ev_loop* loop_;

    // 事件队列
    std::unique_ptr<NotificationQueue<Callback>> queue_;
    std::unique_ptr<FunctionRunner> fn_runner_;
};

#endif

