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

namespace empool
{

    class TaskBase;

    class NoBlockingTaskQueue
    {
    public:

        NoBlockingTaskQueue();
        void Push(TaskBase *task);

        boost::shared_ptr<TaskBase> Pop();

        bool Empty();

        //virtual size_t Size() const;
        ~NoBlockingTaskQueue() {}
    private:
        //BlockingQueue<boost::shared_ptr<TaskBase> > m_tasks;
        Mutex m_mutex;
        ConditionVariable m_cond;
        boost::lockfree::queue<TaskBase *, boost::lockfree::fixed_sized<false> > m_tasks;

    };


}  // namespace empool

#endif
