#include "common/worker_thread.h"

int worker_run(base_worker* worker)
{
    worker_thread::SetCurrentContext(0);
    if (worker)                                                                                                      
    {
        worker->setup();                                                                                             
        worker->run_loop();                                                                                          
        worker->clean_up();                                                                                          
    }
    return 0;
}

worker_thread::worker_thread()
{
    m_worker = 0;
    m_state = 0;
}
worker_thread::worker_thread(base_worker* worker)
{
    m_worker = worker;
    m_state = 0;
}
void worker_thread::Start()
{
    std::lock_guard<std::mutex> guard(m_join_lock);
    if (0==m_state)
    {
        m_state = 1;
        m_thread = std::thread(worker_run, m_worker);
    }
}
/*
void WorkerThread::Stop() {
    
    loop_.RunInEventLoopThread([this]() {
        if (state_ == State::RUNNING) {
            state_ = State::STOP;
            loop_.TerminateLoopSoon();
        } else if (state_ != State::IDLE || state_ != State::STOP) {
            LOG(FATAL) << "Stop() called in unexpected state " 
                       << static_cast<int>(state_);
        }
    });
}

void WorkerThread::Wait() {
    std::lock_guard<std::mutex> lock(join_lock_);
    if (thread_.joinable()) 
        thread_.join();
}

void WorkerThread::Setup() {
    sigset_t ss;
    // Ignore some signals
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
    PCHECK(pthread_sigmask(SIG_BLOCK, &ss, nullptr) == 0);
    // Update the current_worker_ thread-local pointer
    CHECK(current_worker_ == nullptr);
    current_worker_ = this;
}

void WorkerThread::Cleanup() {
    current_worker_ = nullptr;
}

void WorkerThread::RunLoop() {
    // Update state_
    CHECK(state_ == State::STARTING);
    state_ = State::RUNNING;
    VLOG(1) << "WorkerThread " << this << " starting";
    // Call Loop(). This will only return after Stop() has been called.
    loop_.Loop();
    CHECK(state_ == State::STOP);
    state_ = State::IDLE;
    VLOG(1) << "WorkerThread " << this << " terminted";
}*/
