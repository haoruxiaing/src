#include "event_handler.h"
#include <assert.h>
#include <stdlib.h>
#include "event_loop.h"

EventHandler::EventHandler() 
    : loop_(0) {
    ev_io_init(&ev_, &EventHandler::LibevCallback, -1, EventFlags::NONE);
    ev_.data = this;
}

EventHandler::EventHandler(EventLoop* loop, int fd) 
    : loop_(loop) {
    assert(loop_);
    ev_io_init(&ev_, &EventHandler::LibevCallback, fd, EventFlags::NONE);
    ev_.data = this;
}

EventHandler::~EventHandler() {
    UnregisterHandler();
}

void EventHandler::InitHandler(EventLoop* loop, int fd) {
    loop_ = loop;
    assert(loop_);
    ev_io_set(&ev_, fd, ev_.events);
}

void EventHandler::RegisterHandler(int events) {
    ev_io_stop(loop_->GetLibevLoop(), &ev_);

    ev_io_set(&ev_, ev_.fd, events);
    ev_io_start(loop_->GetLibevLoop(), &ev_);
}

void EventHandler::UnregisterHandler() {
    ev_io_stop(loop_->GetLibevLoop(), &ev_);
    ev_.events = EventFlags::NONE;
}

void EventHandler::ChangeHandlerFD(int fd) {
    ev_io_set(&ev_, fd, ev_.events);
}

void EventHandler::LibevCallback(struct ev_loop* loop, struct ev_io* ev, int events) {
    EventHandler* handler = reinterpret_cast<EventHandler*>(ev->data);
    handler->HandlerReady(events);
}

