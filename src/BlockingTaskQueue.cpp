#include "BlockingTaskQueue.h"


namespace zhanmm {



void BlockingTaskQueue::Push(boost::shared_ptr<TaskBase> task)
{
    m_tasks.Push(task);
}

boost::shared_ptr<TaskBase> BlockingTaskQueue::Pop()
{
    return m_tasks.Pop();
}

bool  BlockingTaskQueue::PopTimeWait(boost::shared_ptr<TaskBase>& task, int wait_in_ms) {
    return m_tasks.PopTimeWait(task, wait_in_ms);
}

size_t BlockingTaskQueue::Size() const
{
    return m_tasks.Size();
}

} //namespace zhanmm