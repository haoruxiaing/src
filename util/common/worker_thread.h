#ifndef __WORKER_THREAD_H
#define __WORKER_THREAD_H

#include <mutex>
#include <thread>
#include "base_worker.h"
#include "base_context.h"
class worker_thread
{
public:
    worker_thread();
    worker_thread(base_worker* worker);
    virtual ~worker_thread();
    void Start();
    static base_context* GetCurrentContext() {
        return m_context;
    }
    static void SetCurrentContext(base_context* c){
        m_context = c;
    }
    void  set_worker(base_worker* worker){
        m_worker = worker;
    }
private:
    std::thread                    m_thread;
    static __thread base_context*  m_context;
    unsigned char m_state;
    base_worker*  m_worker;
    std::mutex    m_join_lock;
};

#endif

