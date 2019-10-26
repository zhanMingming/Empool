#include "ScheduledThreadPool.h"


namespace zhanmm {


//typedef ScheduledThreadPool<> LScheduledThreadPool;

  // Implementation

ScheduledThreadPool::ScheduledThreadPool(const size_t threadNum)
: m_stoppedThreadNum(0),
m_state(INIT),
m_index(0),
m_isRequestShutDown(false),
m_handlers(threadNum)
{

    BOOST_FOREACH(boost::shared_ptr<TimerTaskHandler>& t, m_handlers) {
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

  // boost::shared_ptr<TimerTask> ScheduledThreadPool::AddTask(boost::shared_ptr<TimerTask> task)
  // {
  //   return DoAddTask(task);
  // }


  // template<typename Func>
  // boost::shared_ptr<TimerTask> ScheduledThreadPool::AddTask(Func f)
  // {
  //   return DoAddTask(MakeTimerFunctorTask(f));
  // }



  // template<typename T, typename Func>
  // typename FutureTask<T>::Ptr ScheduledThreadPool::AddFutureTask(Func f)
  // {
  //   typename FutureTask<T>::Ptr task(MakeFunctorFutureTask<T>(f).get());
  //   DoAddTask(boost::shared_ptr<TimerTask>(task.get()));
  //   return task;
  // }




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
    if (m_isRequestShutDown.compare_exchange_weak(flag, true, std::memory_order_release, std::memory_order_relaxed)) {
        BOOST_FOREACH(boost::shared_ptr<TimerTaskHandler>& t, m_handlers) {
            bool isStarted = false;
            {
                MutexLocker lock(m_handlerGuard);
                isStarted = static_cast<bool>(t);
            }
            if (isStarted) {
                t->Stop();
            }
        }
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

  
//   void ScheduledThreadPool::StopNow()
//   {
//     StopAsync();

//     BOOST_FOREACH(WorkerThread::Ptr& t, m_threads)
//     {
//         t->CancelAsync();
//     }
    
//     BOOST_FOREACH(WorkerThread::Ptr& t, m_threads)
//     {
//         t->CancelNow();
//     }

//     bool isTimerStarted = false;
//     {
//       MutexLocker lock(m_timerGuard);
//       isTimerStarted = static_cast<bool>(m_timer);
//   }

//     // NOTE: there may be a case that when a thread calling
//     //    AddCronTimerTask and stop running, then this thread calling
//     //    StopNow, then the timer thread runs again.
//     //    In this case, the thread will be created and put a timer task
//     //    but the task will not be put in the thread pool task queue.
//   if (isTimerStarted)
//   {
//       m_timer->Stop(); // wait timer thread to stop
//   }
// }


  // void ScheduledThreadPool::NotifyWhenThreadsStop()
  // {
  //   using boost::bind;

  //   size_t stoppedThreadNum = 0;
  //   {
  //     MutexLocker l(m_stoppedThreadNumGuard);
  //     stoppedThreadNum = ++m_stoppedThreadNum;
  //   }

  //   if (stoppedThreadNum >= m_threads.size())
  //     {
  //       ConditionNotifyAllLocker l(m_stateGuard,
  //                         bind(&Atomic<bool>::GetData,
  //                          &m_isRequestShutDown));
  //   DoSetState(FINISHED);
  //     }
  // }


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


inline boost::shared_ptr<TimerTask> ScheduledThreadPool::AddCronTimerTask(boost::shared_ptr<TimerTask> task,
  TimeValue delay_in_ms)
{
    return DoAddCronTimerTask(task, delay_in_ms);
}


inline boost::shared_ptr<TimerTask> ScheduledThreadPool::AddCycleTimerTask(boost::shared_ptr<TimerTask> task,
  TimeValue period_in_ms, bool is_run_now)
{
    return DoAddCycleTimerTask(task, period_in_ms, is_run_now);
}



inline boost::shared_ptr<TimerTask> ScheduledThreadPool::DoAddCronTimerTask(boost::shared_ptr<TimerTask> task,
  TimeValue delay_in_ms)
{
    if (m_isRequestShutDown) {
      return boost::shared_ptr<TimerTask>();
    } else if (!task) {
      return task;
    }
    int  current_index = m_index.fetch_add(1, std::memory_order_relaxed) % GetThreadNum();
    
    if (m_handlers[current_index]->DoAddCronTimerTask(task, delay_in_ms)) {
        return task;
    } else {
        return boost::shared_ptr<TimerTask>();
    }
    return boost::shared_ptr<TimerTask>();
}


inline boost::shared_ptr<TimerTask> ScheduledThreadPool::DoAddCycleTimerTask(boost::shared_ptr<TimerTask> task,
  TimeValue period_in_ms, bool is_run_now)
{
    if (m_isRequestShutDown) {
        return boost::shared_ptr<TimerTask>();
    } else if (!task) {
        return task;
    }

    int  current_index = m_index.fetch_add(1, std::memory_order_relaxed) % GetThreadNum();
   
    if (m_handlers[current_index]->AddCycleTimerTask(task, period_in_ms, is_run_now)) {
        return task;
    } else {
        return boost::shared_ptr<TimerTask>();
    }
    return boost::shared_ptr<TimerTask>();
}

} //namespace zhanmm
