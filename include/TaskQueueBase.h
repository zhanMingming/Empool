#ifndef ZHANMM_TASK_QUEUE_BASE_H_
#define ZHANMM_TASK_QUEUE_BASE_H_

#include "TaskBase.h"
#include <boost/shared_ptr.hpp>
#include <cstdlib> // for size_t

namespace zhanmm {
  class TaskQueueBase {
  public:

    virtual ~TaskQueueBase() {}
    
    virtual void Push(boost::shared_ptr<TaskBase> task) = 0;
    virtual boost::shared_ptr<TaskBase> Pop() = 0;
    virtual bool PopTimeWait(boost::shared_ptr<TaskBase>& task, int wait_in_ms) = 0;
    virtual size_t Size() const = 0;
  };
}


#endif
