#ifndef ZHANMM_TASK_QUEUE_H_
#define ZHANMM_TASK_QUEUE_H_

#include "TaskQueueBase.h"
#include "BlockQueue.h"
#include "ConditionVariable.h"
#include <queue>
#include <boost/lockfree/queue.hpp>
#include <memory>

namespace zhanmm {

class TaskQueue : public TaskQueueBase {
public:

    TaskQueue() {}
    virtual void Push(boost::shared_ptr<TaskBase> task);
    virtual boost::shared_ptr<TaskBase> Pop();
    virtual size_t Size() const;
    virtual ~TaskQueue() {}
private:
    BlockingQueue<boost::shared_ptr<TaskBase> > m_tasks;
    //boost::lockfree::queue<boost::shared_ptr<TaskBase> > m_tasks;
};


}  // namespace zhanmm

#endif
