#ifndef ZHANMM_SCALING_THREAD_POOL_H_
#define ZHANMM_SCALING_THREAD_POOL_H_

#include "WorkerThread.h"
#include "BlockingTaskQueue.h"
#include "FunctorTask.h"
#include "EndTask.h"
#include "ConditionVariable.h"
#include "Mutex.h"
#include "Util.h"
// #include "Atomic.h"
//#include "FunctorFutureTask.h"
//#include "TimerTaskHandler.h"

#include <atomic>
#include <vector>
#include <cstdlib> // for size_t
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <functional>
#include <unordered_map>


/*
1. Initialize the thread with minthreadsize first.

2. When to add a thread? When the cumulative task of taskqueue reaches a certain threshold, 
increase the thread to ensure the current thread number < maxthreadsize

3. When to reduce the number of threads? When a thread gets a task beyond a certain threshold 
and has not yet got a task, or when it is more than 30s from the last running task, close the thread.

*/

namespace empool
{

    const int  TASK_QUEUE_SIZE_THRESHOLD = 1024;

    typedef boost::function<void()> Function;

    class ScalingThreadPool : public boost::noncopyable
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
        ScalingThreadPool(const size_t minThreadSize = 8,  const size_t maxThreadSize = 16);
        ~ScalingThreadPool();


        size_t GetThreadNum() const;
        size_t GetCorePoolSize() const;
        bool   IfMoreThan() const;
        boost::shared_ptr<TaskBase> AddTask(boost::shared_ptr<TaskBase> task);

        template<typename Func>
        boost::shared_ptr<TaskBase> AddTask(Func f);


        // template <typename T, typename Func>
        // typename FutureTask<T>::Ptr AddFutureTask(Func f);

        void ShutDown();


        void ShutDownNow();


        bool IsShutDown() const;
        //void StopNow();

    private:
        boost::shared_ptr<TaskBase>  DoAddTask(boost::shared_ptr<TaskBase> task);

        void NotifyWhenThreadsStop(int threadId);
        //bool IsShutDown() const;
        bool DoIsShutDown() const;
        void SetState(const State state);
        void DoSetState(const State state);
        bool CheckIsRequestShutDown() const;
        bool AddWorkerThread();
        bool SubWorkerThread(int threadId);
        void AddWorkerThreadIdToSubVector(int threadId);
        void HandleWorkerThread(const Function &checkFunc);
        bool ContraryJudgeFunc();
        bool JudgeFunc();
        void NotifyMonitorThread();


        // member

        State m_state;

        const size_t  m_corePoolSize;

        const size_t  m_maxThreadSize;

        size_t m_stoppedThreadNum;

        std::atomic_bool m_isRequestShutDown;


        // sync tool

        Mutex m_stoppedThreadNumGuard;

        mutable Mutex m_mutex;

        mutable Mutex m_addOrSubThreadNumGuard;

        mutable ConditionVariable m_addOrSubThreadCond;

        mutable ConditionVariable m_stateGuard;


        // stl

        boost::shared_ptr<TaskQueueBase> m_taskQueue;

        std::unordered_map<ThreadId, boost::shared_ptr<WorkerThread> > m_threads;

        std::vector<ThreadId>  m_subWorkerThreadId;

        boost::scoped_ptr<CloseableThread>  m_monitorThread;
    };


    template<typename Func>
    boost::shared_ptr<TaskBase> ScalingThreadPool::AddTask(Func f)
    {
        return DoAddTask(MakeFunctorTask(f));
    }

} //namespace empool

#endif
