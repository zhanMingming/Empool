
#ifndef ZHANMM_FIXED_THREAD_POOL_H_
#define ZHANMM_FIXED_THREAD_POOL_H_

#include "WorkerThread.h"
#include "BlockingTaskQueue.h"
#include "FunctorTask.h"
#include "EndTask.h"
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


namespace zhanmm {
  
template<class TaskQueue = BlockingTaskQueue>
class FixedThreadPool : public boost::noncopyable {
  private:
    enum State {
      INIT,
      RUNNING,
      FINISHED,
    };

  public:
    FixedThreadPool(const size_t threadNum = 8);
    ~FixedThreadPool();
  

    size_t GetThreadNum() const;
    boost::shared_ptr<TaskBase> AddTask(boost::shared_ptr<TaskBase> task);

    template<typename Func>
    boost::shared_ptr<TaskBase> AddTask(Func f);


    // template <typename T, typename Func>
    // typename FutureTask<T>::Ptr AddFutureTask(Func f);

    void Stop();


    void AsyncStop();
    //void StopNow();
    
  private:
    boost::shared_ptr<TaskBase>  DoAddTask(boost::shared_ptr<TaskBase> task);

    void NotifyWhenThreadsStop();
    bool IsFinished() const;
    bool DoIsFinished() const;
    void SetState(const State state);
    void DoSetState(const State state);
    bool CheckIsRequestStop() const;
    // struct ThreadPoolTimerTask;

    // typedef std::vector<boost::shared_ptr<WorkerThread> > WorkerThreads;
    
    //TaskQueueBase::Ptr m_taskQueue;
    boost::shared_ptr<TaskQueueBase> m_taskQueue;
    
    Mutex m_stoppedThreadNumGuard;
    
    size_t m_stoppedThreadNum;
    
    State m_state;
    
    mutable ConditionVariable m_stateGuard;
    
    std::atomic<bool> m_isRequestStop;
    
    std::vector<boost::shared_ptr<WorkerThread> >  m_threads;
    
    Mutex m_mutex;
  };


  typedef FixedThreadPool<> LFixedThreadPool;

  // Implementation
  template<class TaskQueue>
  FixedThreadPool<TaskQueue>::FixedThreadPool(const size_t threadNum)
    : m_mutex(),
      m_stoppedThreadNumGuard(),
      m_stateGuard(m_mutex),
      m_taskQueue(new TaskQueue()),
      m_stoppedThreadNum(0),
      m_state(INIT),
      m_isRequestStop(false),
      m_threads(threadNum)
  {

    BOOST_FOREACH(boost::shared_ptr<WorkerThread>& t, m_threads)
      {
    t.reset(new WorkerThread(m_taskQueue,
                 boost::protect(boost::bind(&FixedThreadPool::
                          NotifyWhenThreadsStop, this))
                 ));
      }

    SetState(RUNNING);
  }
  
  template<class TaskQueue>
  FixedThreadPool<TaskQueue>::~FixedThreadPool()
  {
    // keep other thread from pushing more tasks
    Stop();
  }

  template<class TaskQueue>
  size_t FixedThreadPool<TaskQueue>::GetThreadNum() const
  {
    return m_threads.size();
  }

  template<class TaskQueue>
  boost::shared_ptr<TaskBase> FixedThreadPool<TaskQueue>::AddTask(boost::shared_ptr<TaskBase> task)
  {
    return DoAddTask(task);
  }

  template<class TaskQueue>
  template<typename Func>
  boost::shared_ptr<TaskBase> FixedThreadPool<TaskQueue>::AddTask(Func f)
  {
    return DoAddTask(MakeFunctorTask(f));
  }


  // template<class TaskQueue>
  // template<typename T, typename Func>
  // typename FutureTask<T>::Ptr FixedThreadPool<TaskQueue>::AddFutureTask(Func f)
  // {
  //   typename FutureTask<T>::Ptr task(MakeFunctorFutureTask<T>(f).get());
  //   DoAddTask(boost::shared_ptr<TaskBase>(task.get()));
  //   return task;
  // }


  // 同步stop ，需要等待
  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::Stop()
  {
    using boost::bind;
    using std::mem_fun;
    using std::not1;

    AsyncStop();

    // wait until all worker threads stop
    ConditionWaitLocker l(m_stateGuard,
                bind(not1(mem_fun(&FixedThreadPool::
                          DoIsFinished)),
                     this));
  }


  // 异步stop
  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::AsyncStop()
  {
    bool flag = false;
    if (m_isRequestStop.compare_exchange_weak(flag, true, std::memory_order_release, std::memory_order_relaxed)) {
       
        // NOTE: there may be some tasks pushed after these EndTasks,
        //     because m_isRequestStop is locked alone, and if a thread
        //     calling AddTask before this StopAsync and stop running,
        //     then StopAsync is run, push EndTasks and the thread is
        //     running again, then the tasks will be pushed after the
        //     EndTasks.
      BOOST_FOREACH(boost::shared_ptr<WorkerThread>& t, m_threads)
      {
        t->AsyncClose();
      }
      const size_t threadNum = m_threads.size();
      for (size_t i = 0; i < threadNum; ++i) {
            m_taskQueue->Push(boost::shared_ptr<TaskBase>(new EndTask()));
      }
    }
  }

  // template<class TaskQueue>
  // void FixedThreadPool<TaskQueue>::Stop()
  // {
  //   StopAsync();

  //   BOOST_FOREACH(boost::shared_ptr<WorkerThread>& t, m_threads)
  //     {
  //       t->AsyncClose();
  //     }
    
  //   // BOOST_FOREACH(boost::shared_ptr<WorkerThread>& t, m_threads)
  //   //   {
  //   // t->CancelNow();
  //   //   }
  // }
  // 
  template<class TaskQueue>
  bool FixedThreadPool<TaskQueue>::CheckIsRequestStop() const {
    return m_isRequestStop.load();
  }


  template<class TaskQueue>
  boost::shared_ptr<TaskBase> FixedThreadPool<TaskQueue>::DoAddTask(boost::shared_ptr<TaskBase> task)
  {
    if (m_isRequestStop)
    {
      return boost::shared_ptr<TaskBase>();
    }
    else if (!task)
    {
      return task;
    }
    
    m_taskQueue->Push(task);
    return task;
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::NotifyWhenThreadsStop()
  {

    size_t stoppedThreadNum = 0;
    {
      MutexLocker l(m_stoppedThreadNumGuard);
      stoppedThreadNum = ++m_stoppedThreadNum;
    }

    if (stoppedThreadNum >= m_threads.size())
      {
        ConditionNotifyAllLocker l(m_stateGuard,
                          boost::bind(&FixedThreadPool::CheckIsRequestStop, this));
    DoSetState(FINISHED);
      }
  }

  template<class TaskQueue>
  bool FixedThreadPool<TaskQueue>::IsFinished() const
  {
    MutexLocker l(m_mutex);
    return DoIsFinished();
  }

  template<class TaskQueue>
  bool FixedThreadPool<TaskQueue>::DoIsFinished() const
  {
    return m_state == FINISHED;
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::SetState(const State state)
  {
    MutexLocker l(m_mutex);
    DoSetState(state);
  }

  template<class TaskQueue>
  void FixedThreadPool<TaskQueue>::DoSetState(const State state)
  {
    m_state = state;
  }

}


#endif
