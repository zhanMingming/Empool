#include "ScalingThreadPool.h"

namespace zhanmm {


  //typedef ScalingThreadPool<> LScalingThreadPool;

  // Implementation
  
  ScalingThreadPool::ScalingThreadPool(const size_t minThreadSize,  const size_t maxThreadSize)
    : m_mutex(),
      m_corePoolSize(minThreadSize),
      m_maxThreadSize(maxThreadSize),
      m_stoppedThreadNumGuard(),
      m_addOrSubThreadNumGuard(),
      m_stateGuard(m_mutex),
      m_taskQueue(new BlockingTaskQueue()),
      m_stoppedThreadNum(0),
      m_state(INIT),
      m_isRequestShutDown(false),
      m_threads(m_corePoolSize)
  {

    BOOST_FOREACH(boost::shared_ptr<WorkerThread>& t, m_threads)
      {
    t.reset(new WorkerThread(m_taskQueue, boost::protect(boost::bind(&ScalingThreadPool::
                          NotifyWhenThreadsStop, this))
                 ));
      }

    SetState(RUNNING);
    
  }
  
  
  ScalingThreadPool::~ScalingThreadPool()
  {
    // keep other thread from pushing more tasks
    ShutDown();
  }

  
  size_t ScalingThreadPool::GetThreadNum() const
  {
    MutexLocker lock(m_addOrSubThreadNumGuard);
    return m_threads.size();
  }

  
  boost::shared_ptr<TaskBase> ScalingThreadPool::AddTask(boost::shared_ptr<TaskBase> task)
  {
    return DoAddTask(task);
  }

  
  template<typename Func>
  boost::shared_ptr<TaskBase> ScalingThreadPool::AddTask(Func f)
  {
    return DoAddTask(MakeFunctorTask(f));
  }


  // 
  // template<typename T, typename Func>
  // typename FutureTask<T>::Ptr ScalingThreadPool::AddFutureTask(Func f)
  // {
  //   typename FutureTask<T>::Ptr task(MakeFunctorFutureTask<T>(f).get());
  //   DoAddTask(boost::shared_ptr<TaskBase>(task.get()));
  //   return task;
  // }


  // 同步stop ，需要等待
  
  void ScalingThreadPool::ShutDownNow()
  {
    using boost::bind;
    using std::mem_fun;
    using std::not1;

    ShutDown();

    // wait until all worker threads stop
    ConditionWaitLocker l(m_stateGuard,
                bind(not1(mem_fun(&ScalingThreadPool::
                          DoIsShutDown)),
                     this));
  }


  // 异步stop
  
  void ScalingThreadPool::ShutDown()
  {
    bool flag = false;
    if (m_isRequestShutDown.compare_exchange_weak(flag, true, std::memory_order_release, std::memory_order_relaxed)) {
      
      SetState(STOP);
        // NOTE: there may be some tasks pushed after these EndTasks,
        //     because Sm_isRequestShutDown is locked alone, and if a thread
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

  // 
  // void ScalingThreadPool::Stop()
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

  bool ScalingThreadPool::CheckIsRequestShutDown() const {
    return m_isRequestShutDown.load();
  }

  bool  ScalingThreadPool::AddWorkThread() {
    if (m_taskQueue->Size() < TASK_QUEUE_SIZE_THRESHOLD || GetThreadNum() >= m_maxThreadSize || m_isRequestShutDown.load()) {
        return false;
    }

    boost::shared_ptr<WorkerThread>  newWorkerThread(new WorkerThread(m_taskQueue, boost::protect(boost::bind(&ScalingThreadPool::
                          NotifyWhenThreadsStop, this))));
    

    MutexLocker   lock(m_addOrSubThreadNumGuard);
    m_threads.push_back(newWorkerThread);
    return true;

  }

  
  boost::shared_ptr<TaskBase> ScalingThreadPool::DoAddTask(boost::shared_ptr<TaskBase> task)
  {
    if (m_isRequestShutDown.load())
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

  
  void ScalingThreadPool::NotifyWhenThreadsStop()
  {

    size_t stoppedThreadNum = 0;
    {
      MutexLocker l(m_stoppedThreadNumGuard);
      stoppedThreadNum = ++m_stoppedThreadNum;
    }

    if (stoppedThreadNum >= m_threads.size())
    {
        //DoSetState(FINISHED);
        SetState(SHUTDOWN);
        ConditionNotifyAllLocker l(m_stateGuard,
                          boost::bind(&ScalingThreadPool::CheckIsRequestShutDown, this));
        //DoSetState(FINISHED);
    }
  }

  
  bool ScalingThreadPool::IsShutDown() const
  {
    MutexLocker lock(m_mutex);
    return DoIsShutDown();
  }

  
  bool ScalingThreadPool::DoIsShutDown() const
  {
    return m_state == SHUTDOWN;
  }

  
  void ScalingThreadPool::SetState(const State state)
  {
    MutexLocker lock(m_mutex);
    DoSetState(state);
  }

  
  void ScalingThreadPool::DoSetState(const State state)
  {
    m_state = state;
  }

} // namespace zhanmm

