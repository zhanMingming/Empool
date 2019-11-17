#include "NoBlockingTaskQueue.h"
#include "TaskBase.h"
#include <assert.h>
#include <iostream>

namespace empool
{

    NoBlockingTaskQueue::NoBlockingTaskQueue()
        : m_mutex(), m_cond(m_mutex), m_tasks(1024)
    {}



    void NoBlockingTaskQueue::Push(TaskBase *task)
    {
        // bool notify = Empty();
        //std::cout << "push task" << std::endl;
        m_tasks.push(task);
        //m_cond.Notify();
        // if (notify) {
        //     std::cout << "notifyAll" << std::endl;
        //     m_cond.Notify();
        // }
    }


    bool NoBlockingTaskQueue::Empty()
    {
        return m_tasks.empty();
    }


    boost::shared_ptr<TaskBase> NoBlockingTaskQueue::Pop()
    {
        // //唤醒一个线程
        TaskBase *p = nullptr;
        // if (m_tasks.empty()) {
        //     std::cout << "wait here" << std::endl;
        //     m_cond.Wait();
        // }

        while(!m_tasks.pop(p));
        // std::cout << "get task" << std::endl;
        assert(p != nullptr);
        boost::shared_ptr<TaskBase> ret(p);
        return ret;
    }

    // size_t NoBlockingTaskQueue::Size() const
    // {
    //     return m_tasks.size();
    // }

} //namespace empool
