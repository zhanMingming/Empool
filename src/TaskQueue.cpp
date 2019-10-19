#include "TaskQueue.h"


namespace zhanmm {



void TaskQueue::Push(boost::shared_ptr<TaskBase> task)
{
    m_tasks.Push(task);
}

boost::shared_ptr<TaskBase> TaskQueue::Pop()
{
    return m_tasks.Pop();
}

size_t TaskQueue::Size() const
{
    return m_tasks.Size();
}

} //namespace zhanmm