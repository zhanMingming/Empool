#include "TimerTaskQueue.h"

#include <boost/bind.hpp>
#include <cassert>
#include <iostream>
#include <sys/time.h>

using namespace std;
namespace zhanmm
{

    /// Timer::TimerTaskQueue
    boost::shared_ptr<TimerTask> TimerTaskQueue::GetMin() const
    {
        assert(!m_queue.empty());
        return m_queue.top();
    }

    boost::shared_ptr<TimerTask> TimerTaskQueue::PopMin()
    {
        assert(!m_queue.empty());
        boost::shared_ptr<TimerTask> min_task = m_queue.top();
        m_queue.pop();
        return min_task;
    }

    void TimerTaskQueue::PopMinAndPush()
    {
        assert(!m_queue.empty());
        // the deadline in the min_task should be changed
        boost::shared_ptr<TimerTask> min_task = m_queue.top();
        m_queue.pop();
        m_queue.push(min_task);
    }

    /// precondition: the mutex associated with this queue
    ///    should be locked
    void TimerTaskQueue::PushTask(boost::shared_ptr<TimerTask> task)
    {
        m_queue.push(task);
        // there is only one thread(the timer thread)
        // waiting for this condition, so it's okay
        // to use notify.
        m_cond.Notify();
    }


    bool TimerTaskQueue::IsEmpty() const
    {
        return m_queue.empty();
    }

    TimerTaskQueue::TimerTaskQueue(Mutex &m)
        : m_cond(m), m_queue(TimerTaskQueue::CompareTimerTask)
    {}

    unsigned TimerTaskQueue::GetSize() const
    {
        return static_cast<unsigned>(m_queue.size());
    }

    void TimerTaskQueue::Wait()
    {
        m_cond.Wait();
    }

    void TimerTaskQueue::Notify()
    {
        m_cond.Notify();
    }


    bool TimerTaskQueue::TimedWait(TimeValue delay)
    {
        return m_cond.TimeWait(delay);
    }

    /// precondition: the mutex associated with this queue
    ///    should be locked
    void TimerTaskQueue::Clear()
    {
        while (!m_queue.empty())
        {
            m_queue.pop();
        }
        //m_cond.Notify();
    }

    bool TimerTaskQueue::CompareTimerTask(
        boost::shared_ptr<TimerTask> a, boost::shared_ptr<TimerTask> b)
    {
        return a->GetDeadline() > b->GetDeadline();
    }


} //namespace zhanmm
