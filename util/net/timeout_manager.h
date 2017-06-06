#ifndef NET_TIMEOUT_MANAGER_H
#define NET_TIMEOUT_MANAGER_H

#include <chrono>

class AsyncTimeout;

// 定时器管理接口定义，AsyncTimeout将用这些接口去调度/关闭定时器。
class TimeoutManager {
 public:
  typedef std::chrono::milliseconds timeout_type;
  virtual ~TimeoutManager() { }

  // 调度定时器
  // 
  // @param timeout 超时时间(毫秒)
  //
  virtual void ScheduleTimeout(AsyncTimeout* obj, timeout_type timeout) = 0;
  // 关闭定时器
  virtual void CancelTimeout(AsyncTimeout* obj) = 0;


  // 判断是否运行在定时器管理线程上
  virtual bool IsInTimeoutManagerThread() = 0;

};

#endif

