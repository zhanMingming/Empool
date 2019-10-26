#ifndef ZHANMM_HIGH_LEVEL_TASK_QUEUE_H_
#define ZHANMM_HIGH_LEVEL_TASK_QUEUE_H_

// #include "TaskQueueBase.h"
// #include "BlockQueue.h"
// #include "ConditionVariable.h"
// #include <queue>
#include <boost/lockfree/queue.hpp>
#include <boost/shared_ptr.hpp>
#include "Mutex.h"
#include "ConditionVariable.h"

namespace zhanmm {

class TaskBase;

class HighPerformanceTaskQueue {
public:

    HighPerformanceTaskQueue();
    void Push(TaskBase* task);
    
    boost::shared_ptr<TaskBase> Pop();

    bool Empty();

    //virtual size_t Size() const;
    ~HighPerformanceTaskQueue() {}
private:
    //BlockingQueue<boost::shared_ptr<TaskBase> > m_tasks;
    boost::lockfree::queue<TaskBase*, boost::lockfree::fixed_sized<false> > m_tasks;
    ConditionVariable m_cond;
    Mutex m_mutex;

};


}  // namespace zhanmm

#endif
