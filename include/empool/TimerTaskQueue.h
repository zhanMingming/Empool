#ifndef ZHANMM_TIMER_TASK_QUEUE_H_
#define ZHANMM_TIMER_TASK_QUEUE_H_

#include "ConditionVariable.h"
#include "TaskBase.h"
#include "Mutex.h"
#include "TimerTask.h"

#include <memory>
#include <queue>
#include <exception>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


namespace empool
{

    class TimerTaskQueue
    {
    public:
        TimerTaskQueue(Mutex &m);
        ~TimerTaskQueue();
        boost::shared_ptr<TimerTask> GetMin() const;
        boost::shared_ptr<TimerTask> PopMin();
        void PopMinAndPush();
        void PushTask(boost::shared_ptr<TimerTask> task);
        bool IsEmpty() const;
        unsigned GetSize() const;
        void Clear();
        void Wait();
        void Notify();

        /// return true when the condition is signaled,
        /// otherwise return false
        bool TimedWait(TimeValue delay);

    private:
        static bool CompareTimerTask(boost::shared_ptr<TimerTask> a, boost::shared_ptr<TimerTask> b);

        typedef bool (*CompareFunc)(boost::shared_ptr<TimerTask>, boost::shared_ptr<TimerTask>);
        typedef std::priority_queue<boost::shared_ptr<TimerTask>,
                std::vector<boost::shared_ptr<TimerTask> >, CompareFunc> Queue;

        mutable ConditionVariable m_cond;
        Queue m_queue;
    };

} //namespace empool


#endif  // ZHANMM_TIMER_TASK_QUEUE_H_


