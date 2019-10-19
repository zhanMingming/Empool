#ifndef ZHANMM_HIGH_LEVEL_TASK_QUEUE_H_
#define ZHANMM_HIGH_LEVEL_TASK_QUEUE_H_

// #include "TaskQueueBase.h"
// #include "BlockQueue.h"
// #include "ConditionVariable.h"
// #include <queue>
#include <boost/lockfree/queue.hpp>
#include <boost/shared_ptr.hpp>

namespace zhanmm {

class TaskBase;

class HighPerformanceTaskQueue {
public:

    HighPerformanceTaskQueue():m_tasks(1024) {}
    void Push(TaskBase* task);
    
    boost::shared_ptr<TaskBase> Pop();

    //virtual size_t Size() const;
    ~HighPerformanceTaskQueue() {}
private:
    //BlockingQueue<boost::shared_ptr<TaskBase> > m_tasks;
    boost::lockfree::queue<TaskBase*, boost::lockfree::fixed_sized<false> > m_tasks;
};


}  // namespace zhanmm

#endif
