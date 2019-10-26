#ifndef ZHANMM_TIMER_TASK_HANDLER_EVENT_H_
#define ZHANMM_TIMER_TASK_HANDLER_EVENT_H_

#include "CloseableThread.h"
#include "ConditionVariable.h"
#include "TaskBase.h"
#include "Mutex.h"
#include "TimerTaskQueue.h"
#include "TimerTask.h"

#include <memory>
#include <queue>
#include <exception>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>


namespace zhanmm {


TimeValue GetCurrentTime();


class TimerTaskHandler : public boost::noncopyable {
public:
    TimerTaskHandler();
    ~TimerTaskHandler();

    /// run the task after delay_in_ms.
    bool AddCronTimerTask(boost::shared_ptr<TimerTask> task, TimeValue delay_in_ms);

    /// run the task every interval_in_ms.
    /// If is_run_now is true, the task will be run immediately
    bool AddCycleTimerTask(boost::shared_ptr<TimerTask> task, TimeValue interval_in_ms,
            bool is_run_now);

    template<typename Func>
    boost::shared_ptr<TimerTask> AddCronTimerTask(Func func, TimeValue delay_in_ms);

    template<typename Func>
    boost::shared_ptr<TimerTask> AddCycleTimerTask(Func func, TimeValue interval_in_ms,
            bool is_run_now);

    //void StopAsync();
    void Stop();

private:
    bool DoAddCronTimerTask(boost::shared_ptr<TimerTask> task, TimeValue delay_in_ms);
    bool DoAddCycleTimerTask(boost::shared_ptr<TimerTask> task, TimeValue interval_in_ms,
            bool is_run_now);

    typedef CloseableThread::Function Function;
    void ThreadFunction(const Function& checkFunc);
    void ProcessError(const std::exception& e);

    mutable Mutex m_queue_guard;
    TimerTaskQueue m_task_queue;
    boost::scoped_ptr<CloseableThread> m_thread;
};


template<typename Func>
inline boost::shared_ptr<TimerTask> TimerTaskHandler::AddCronTimerTask(Func func, TimeValue delay_in_ms)
{
    boost::shared_ptr<TimerTask> task(MakeTimerFunctorTask(func));
    if (!DoAddCronTimerTask(task, delay_in_ms))
    {
        task.reset();
    }
    return task;
}


template<typename Func>
inline boost::shared_ptr<TimerTask>
TimerTaskHandler::AddCycleTimerTask(Func func, TimeValue interval_in_ms, bool is_run_now)
{
    boost::shared_ptr<TimerTask> task(MakeTimerFunctorTask(func));
    if (!DoAddCycleTimerTask(task, interval_in_ms, is_run_now))
    {
        task.reset();
    }
    return task;
}


}  // namespace zhanmm


#endif 


