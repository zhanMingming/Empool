#ifndef ZHANMM_TIME_TASK_H_
#define ZHANMM_TIME_TASK_H_

#include "CloseableThread.h"
#include "ConditionVariable.h"
#include "TaskBase.h"
#include "Mutex.h"
#include "Util.h"

#include <memory>
#include <queue>
#include <exception>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


namespace empool
{



    //TimeValue GetCurrentTime();

    class TimerTaskHandler;

    /// Subclass should override the DoRun function
    class TimerTask : public TaskBase
    {
        friend class TimerTaskHandler;

    public:
        //typedef boost::shared_ptr<TimerTask> Ptr;

        TimerTask();

        TimeValue GetDeadline() const;

    private:
        void SetDeadline(const TimeValue deadline);
        bool IsIntervalTask() const;
        TimeValue GetInterval() const;
        void SetInterval(const TimeValue interval);

        TimeValue m_deadline;
        TimeValue m_interval;
        mutable Mutex m_time_guard;
    };

    class EndTimerTask : public TimerTask
    {
        public:
            virtual void DoRun() {}
    };
    

    template<typename Func>
    class TimerFunctorTask : public TimerTask
    {
    public:
        TimerFunctorTask(Func f);

        virtual void DoRun();

    private:
        Func m_functor;
    };

    /// helper function
    template<typename Func>
    boost::shared_ptr<TimerTask> MakeTimerFunctorTask(Func f)
    {
        return boost::shared_ptr<TimerTask>(new TimerFunctorTask<Func>(f));
    }


    /// Implementation
    template<typename Func>
    inline TimerFunctorTask<Func>::TimerFunctorTask(Func f)
        : m_functor(f)
    {}

    template<typename Func>
    inline void TimerFunctorTask<Func>::DoRun()
    {
        m_functor();
    }


}  // namespace empool

#endif  // ZHANMM_TIMER_H_
