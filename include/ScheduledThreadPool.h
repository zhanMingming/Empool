
#ifndef ZHANMM_Scheduled_THREAD_POOL_H_
#define ZHANMM_Scheduled_THREAD_POOL_H_

#include "WorkerThread.h"
//#include "TaskQueue.h"
#include "FunctorTask.h"
// #include "EndTask.h"
#include "ConditionVariable.h"
// #include "Atomic.h"
//#include "FunctorFutureTask.h"
#include "TimerTaskHandler.h"

#include <atomic>
#include <vector>
#include <cstdlib> // for size_t
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <functional>


namespace zhanmm
{


    class ScheduledThreadPool : public boost::noncopyable
    {
    private:
        enum State
        {
            INIT,
            RUNNING,
            STOP,
            SHUTDOWN
        };

    public:
        ScheduledThreadPool(const size_t threadNum = 4);
        ~ScheduledThreadPool();


        size_t GetThreadNum() const;


        boost::shared_ptr<TimerTask> AddCronTimerTask(boost::shared_ptr<TimerTask> task,
                TimeValue delay_in_ms);


        boost::shared_ptr<TimerTask> AddCycleTimerTask(boost::shared_ptr<TimerTask> task,
                TimeValue period_in_ms, bool is_run_now);

        template<typename Func>
        boost::shared_ptr<TimerTask> AddCronTimerTask(Func f, TimeValue delay_in_ms);

        template<typename Func>
        boost::shared_ptr<TimerTask> AddCycleTimerTask(Func f,
                TimeValue period_in_ms, bool is_run_now = true);

        //异步关闭，如果有task 正在执行，则等待task 完成后，进行关闭
        void ShutDown();
        //void StopAsync();
        //
        void ShutDownNow();
        //
        bool IsShutDown() const;

    private:

        boost::shared_ptr<TimerTask> DoAddCronTimerTask(boost::shared_ptr<TimerTask> task,
                TimeValue delay_in_ms);

        boost::shared_ptr<TimerTask> DoAddCycleTimerTask(boost::shared_ptr<TimerTask> task,
                TimeValue period_in_ms, bool is_run_now);


        //void NotifyWhenThreadsStop();
        //bool IsFinished() const;
        bool DoIsShutDown() const;
        void SetState(const State state);
        void DoSetState(const State state);
        void ShutDownHelper(bool now = false);


        State m_state;

        std::atomic_long m_index;
        
        mutable Mutex m_stateGuard;

        std::atomic_bool m_isRequestShutDown;


        Mutex m_handlerGuard;

        std::vector<boost::shared_ptr<TimerTaskHandler> > m_handlers;
    };


    template<typename Func>
    inline boost::shared_ptr<TimerTask> ScheduledThreadPool::AddCronTimerTask(Func f, TimeValue delay_in_ms)
    {
        boost::shared_ptr<TimerTask> task(MakeTimerFunctorTask(f));
        return DoAddCronTimerTask(task, delay_in_ms);
    }


    template<typename Func>
    inline boost::shared_ptr<TimerTask> ScheduledThreadPool::AddCycleTimerTask(Func f, TimeValue period_in_ms, bool is_run_now)
    {
        boost::shared_ptr<TimerTask> task(MakeTimerFunctorTask(f));
        return DoAddCycleTimerTask(task, period_in_ms, is_run_now);
    }


} //namespace zhanmm

#endif
