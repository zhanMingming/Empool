#include "TimerTaskHandler.h"

#include <boost/bind.hpp>
#include <cassert>
#include <iostream>
#include <sys/time.h>

using namespace std;
namespace zhanmm
{


    TimeValue GetCurrentTime()
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        TimeValue now_in_ms = now.tv_sec;
        now_in_ms = now_in_ms * 1000 + now.tv_usec / 1000;
        return now_in_ms;
    }


    /// TimerTaskHandler
    TimerTaskHandler::TimerTaskHandler()
        : m_task_queue(m_queue_guard)
    {

        // ensure that the thread is created successfully.
        while (true)
        {
            try
            {
                // check for the creation exception
                m_thread.reset(new CloseableThread(
                                   BOOST_BIND(&TimerTaskHandler::ThreadFunction, this, _1)));
                break;
            }
            catch (const std::exception &e)
            {
                ProcessError(e);
            }
        }
    }


    TimerTaskHandler::~TimerTaskHandler()
    {
        Stop();
    }

    void TimerTaskHandler::ThreadFunction(const Function &checkFunc)
    {
        while (true)
        {
            bool is_fired = false;
            boost::shared_ptr<TimerTask> task;

            {
                MutexLocker lock(m_queue_guard);
                while (m_task_queue.IsEmpty())
                {
                    checkFunc();
                    std::cout << "thread wait" << std::endl;
                    m_task_queue.TimedWait(MAX_WAIT_TIME_MS_WEHN_QUEUE_IS_EMPTY);
                }

                checkFunc();

                task = m_task_queue.GetMin();

                const TimeValue now = GetCurrentTime();
                const TimeValue deadline = task->GetDeadline();
                if (task->IsCancelled())
                {
                    (void) m_task_queue.PopMin();
                }
                else if (task->IsRequestCancel())
                {
                    is_fired = true;
                    (void) m_task_queue.PopMin();
                }
                else if (deadline <= now)
                {
                    is_fired = true;
                    if (task->IsIntervalTask())
                    {
                        task->SetDeadline(now + task->GetInterval());
                        m_task_queue.PopMinAndPush();
                    }
                    else
                    {
                        (void) m_task_queue.PopMin();
                    }
                }
                else
                {
                    const TimeValue delay = deadline - now;
                    (void) m_task_queue.TimedWait(delay);
                }
            } // mutex unlock

            if (is_fired)
            {
                task->Run();
            }
        }
    }

    bool TimerTaskHandler::AddCronTimerTask(boost::shared_ptr<TimerTask> task, TimeValue delay_in_ms)
    {
        return DoAddCronTimerTask(task, delay_in_ms);
    }

    bool TimerTaskHandler::AddCycleTimerTask(boost::shared_ptr<TimerTask> task, TimeValue interval_in_ms,
            bool is_run_now)
    {
        return DoAddCycleTimerTask(task, interval_in_ms, is_run_now);
    }


    bool TimerTaskHandler::DoAddCronTimerTask(boost::shared_ptr<TimerTask> task, TimeValue delay_in_ms)
    {
        const TimeValue now = GetCurrentTime();
        const TimeValue task_deadline = now + delay_in_ms;

        if (m_thread->IsRequestClose())
        {
            return false;
        }

        // A task having been put in the TimerTaskHandler cannot be put again
        if (task->GetDeadline() != 0)
        {
            return false;
        }

        task->SetDeadline(task_deadline);
        MutexLocker lock(m_queue_guard);
        m_task_queue.PushTask(task);

        return true;
    }







    bool TimerTaskHandler::DoAddCycleTimerTask(boost::shared_ptr<TimerTask> task, TimeValue interval_in_ms,
            bool is_run_now)
    {
        const TimeValue now = GetCurrentTime();

        if (m_thread->IsRequestClose())
        {
            return false;
        }

        // A task having been put in the TimerTaskHandler cannot be put again
        if (task->GetDeadline() != 0)
        {
            return false;
        }

        if (is_run_now)
        {
            task->SetDeadline(now);
        }
        else
        {
            task->SetDeadline(now + interval_in_ms);
        }
        task->SetInterval(interval_in_ms);
        MutexLocker lock(m_queue_guard);

        m_task_queue.PushTask(task);

        return true;
    }



    // void TimerTaskHandler::StopAsync()
    // {
    //     m_thread->CancelAsync();
    //     sync::MutexLocker lock(m_queue_guard);
    //     m_TimerTaskHandler_queue.Clear();
    // }

    void TimerTaskHandler::Stop()
    {
        m_thread->AsyncClose();
        std::cout << "asyncClose" << std::endl;
        m_task_queue.Notify();
        std::cout << "Notify" << std::endl;
        m_thread->Close();
        std::cout << "close" << std::endl;
        MutexLocker lock(m_queue_guard);
        std::cout << "lock" << std::endl;
        m_task_queue.Clear();
        std::cout << "clear" << std::endl;
    }


    void TimerTaskHandler::ProcessError(const std::exception &e)
    {
        cerr << "TimerTaskHandler ctor" << endl;
        cerr << e.what() << endl;
        cerr << "Try again." << endl;
    }


} //namespace zhanmm
