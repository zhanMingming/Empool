#include "HighPerformanceTaskQueue.h"
#include "TaskBase.h"
#include <assert.h>


namespace zhanmm {




void HighPerformanceTaskQueue::Push(TaskBase* task)
{
    m_tasks.push(task);
}

boost::shared_ptr<TaskBase> HighPerformanceTaskQueue::Pop()
{
    TaskBase* p = nullptr;
    // while(!m_tasks.empty()) {
    while(!m_tasks.pop(p));
    assert(p != nullptr);
    boost::shared_ptr<TaskBase> ret(p);
    return ret;
}

// size_t HighPerformanceTaskQueue::Size() const
// {
//     return m_tasks.size();
// }

} //namespace zhanmm
