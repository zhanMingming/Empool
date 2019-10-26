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

size_t BlockingTaskQueue::Size() const
{
    return m_tasks.Size();
}

} //namespace zhanmm