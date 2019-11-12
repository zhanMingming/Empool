#ifndef ZHANMM_TASK_QUEUE_H_
#define ZHANMM_TASK_QUEUE_H_

#include "TaskQueueBase.h"
#include "BlockingQueue.h"
#include "ConditionVariable.h"
#include <queue>
#include <boost/lockfree/queue.hpp>
#include <memory>

namespace empool {

class BlockingTaskQueue : public TaskQueueBase {
public:

    BlockingTaskQueue() {}
    virtual void Push(boost::shared_ptr<TaskBase> task);
    virtual boost::shared_ptr<TaskBase> Pop();

    bool PopTimeWait(boost::shared_ptr<TaskBase>& task, int wait_in_ms);
    virtual size_t Size() const;
    virtual ~BlockingTaskQueue() {}
private:
    BlockingQueue<boost::shared_ptr<TaskBase> > m_tasks;
    //boost::lockfree::queue<boost::shared_ptr<TaskBase> > m_tasks;
};


}  // namespace empool

#endif
