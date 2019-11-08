#include "ScheduledThreadPool.h"
#include <iostream>

namespace zhanmm
{


    //typedef ScheduledThreadPool<> LScheduledThreadPool;

    // Implementation

    ScheduledThreadPool::ScheduledThreadPool(const size_t threadNum)
        : m_state(INIT),
          m_index(0),
          m_isRequestShutDown(false),
          m_handlers(threadNum)
    {

        BOOST_FOREACH(boost::shared_ptr<TimerTaskHandler> &t, m_handlers)
        {
            t.reset(new TimerTaskHandler());
        }

        SetState(RUNNING);
    }


    ScheduledThreadPool::~ScheduledThreadPool()
    {
        // keep other thread from pushing more tasks
        ShutDown();
    }


    size_t ScheduledThreadPool::GetThreadNum() const
    {
        return m_handlers.size();
    }


    void ScheduledThreadPool::ShutDownNow()
    {
        ShutDown();
    }


    void ScheduledThreadPool::ShutDown()
    {
        //     using boost::bind;
        //     using std::mem_fun;
        //     using std::not1;

        //     StopAsync();

        //     // wait until all worker threads stop
        //     // ConditionWaitLocker l(m_stateGuard,
        //     //             bind(not1(mem_fun(&ScheduledThreadPool::
        //     //                       DoIsFinished)),
        //     //                  this));

        //     // bool isTimerStarted = false;
        //     // {
        //     //   MutexLocker lock(m_timerGuard);
        //     //   isTimerStarted = static_cast<bool>(m_timer);
        //     // }

        //     if (isTimerStarted)
        //     {
        //       m_timer->Stop(); // wait timer thread to stop
        //     }
        bool flag = false;
        if (m_isRequestShutDown.compare_exchange_weak(flag, true, std::memory_order_release, std::memory_order_relaxed))
        {
            BOOST_FOREACH(boost::shared_ptr<TimerTaskHandler> &t, m_handlers)
            {
                bool isStarted = false;
                {
                    MutexLocker lock(m_handlerGuard);
                    isStarted = static_cast<bool>(t);
                }
                if (isStarted)
                {
                    std::cout << "start stop" << std::endl;
                    t->Stop();
                }
            }
            SetState(FINISHED);
        }
    }


    // void ScheduledThreadPool::StopAsync()
    // {
    //     if (m_isRequestShutDown.CompareAndSet(false, true))
    //     {
    //         // Stop the timer first
    //         bool isTimerStarted = false;
    //         {
    //           MutexLocker lock(m_timerGuard);
    //           isTimerStarted = static_cast<bool>(m_timer);
    //       }

    //       if (isTimerStarted)
    //       {
    //           m_timer->StopAsync();
    //       }

    //         // NOTE: there may be some tasks pushed after these EndTasks,
    //         //     because m_isRequestShutDown is locked alone, and if a thread
    //         //     calling AddTask before this StopAsync and stop running,
    //         //     then StopAsync is run, push EndTasks and the thread is
    //         //     running again, then the tasks will be pushed after the
    //         //     EndTasks.
    //     // const size_t threadNum = m_threads.size();
    //     // for (size_t i = 0; i < threadNum; ++i)
    //     //   {
    //     //     m_taskQueue->Push(boost::shared_ptr<TimerTask>(new EndTask));
    //     //   }
    //     //   }
    //   }



    bool ScheduledThreadPool::IsShutDown() const
    {
        return IsFinished();
    }

    bool ScheduledThreadPool::IsFinished() const
    {
        MutexLocker l(m_stateGuard);
        return DoIsFinished();
    }


    bool ScheduledThreadPool::DoIsFinished() const
    {
        return m_state == FINISHED;
    }


    void ScheduledThreadPool::SetState(const State state)
    {
        MutexLocker l(m_stateGuard);
        DoSetState(state);
    }


    void ScheduledThreadPool::DoSetState(const State state)
    {
        m_state = state;
    }


    boost::shared_ptr<TimerTask> ScheduledThreadPool::AddCronTimerTask(boost::shared_ptr<TimerTask> task,
            TimeValue delay_in_ms)
    {
        return DoAddCronTimerTask(task, delay_in_ms);
    }


    boost::shared_ptr<TimerTask> ScheduledThreadPool::AddCycleTimerTask(boost::shared_ptr<TimerTask> task,
            TimeValue period_in_ms, bool is_run_now)
    {
        return DoAddCycleTimerTask(task, period_in_ms, is_run_now);
    }



    boost::shared_ptr<TimerTask> ScheduledThreadPool::DoAddCronTimerTask(boost::shared_ptr<TimerTask> task,
            TimeValue delay_in_ms)
    {
        if (m_isRequestShutDown)
        {
            return boost::shared_ptr<TimerTask>();
        }
        else if (!task)
        {
            return task;
        }
        int  current_index = m_index.fetch_add(1, std::memory_order_relaxed) % GetThreadNum();

        if (m_handlers[current_index]->AddCronTimerTask(task, delay_in_ms))
        {
            return task;
        }
        else
        {
            return boost::shared_ptr<TimerTask>();
        }
        return boost::shared_ptr<TimerTask>();
    }


    boost::shared_ptr<TimerTask> ScheduledThreadPool::DoAddCycleTimerTask(boost::shared_ptr<TimerTask> task,
            TimeValue period_in_ms, bool is_run_now)
    {
        if (m_isRequestShutDown)
        {
            return boost::shared_ptr<TimerTask>();
        }
        else if (!task)
        {
            return task;
        }

        int  current_index = m_index.fetch_add(1, std::memory_order_relaxed) % GetThreadNum();

        if (m_handlers[current_index]->AddCycleTimerTask(task, period_in_ms, is_run_now))
        {
            return task;
        }
        else
        {
            return boost::shared_ptr<TimerTask>();
        }
        return boost::shared_ptr<TimerTask>();
    }

} //namespace zhanmm
