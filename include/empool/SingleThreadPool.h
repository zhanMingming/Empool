#ifndef ZHANMM_Single_THREAD_POOL_H_
#define ZHANMM_Single_THREAD_POOL_H_

#include "CloseableThread.h"
//#include "TaskQueue.h"
// #include "EndTask.h"
#include "ConditionVariable.h"
#include "Util.h"
#include "TaskBase.h"

#include <atomic>
#include <vector>
#include <cstdlib> // for size_t
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <functional>
#include <queue>


namespace empool
{


    
    class PriorityTask : public TaskBase
    {
    public:
        PriorityTask()
            : m_priority(0)
        {

        }

        void SetPriorityValue(long priority)
        {
            m_priority = priority;
        }

        long GetPriorityValue()
        {
            return m_priority;
        }

    private:
        long  m_priority;
    };

    class EndPriorityTask : public PriorityTask
    {
        public :
        virtual void DoRun() {}
    };


    template<typename Func>
    class PriorityFunctorTask : public PriorityTask
    {
    public:
        PriorityFunctorTask(Func f, long priority);

        virtual void DoRun();

    private:
        Func m_functor;
    };




    /// helper function
    template<typename Func>
    boost::shared_ptr<PriorityTask> MakePriorityFunctorTask(Func f, long priority)
    {
        return boost::shared_ptr<PriorityTask>(new PriorityFunctorTask<Func>(f, priority));
    }


    /// Implementation
    template<typename Func>
    inline PriorityFunctorTask<Func>::PriorityFunctorTask(Func f, long priority)
        : m_functor(f)
    {
        SetPriorityValue(priority);
    }

    template<typename Func>
    inline void PriorityFunctorTask<Func>::DoRun()
    {
        m_functor();
    }


    enum RunMode
    {
        FIFO,
        LIFO,
        PRIORITY
    };


    class SingleThreadPool : public boost::noncopyable
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

        typedef CloseableThread::Function Function;


        SingleThreadPool(RunMode mode);
        ~SingleThreadPool();


        size_t GetThreadNum() const;

        // boost::shared_ptr<TaskBase> AddTask(boost::shared_ptr<TaskBase> task);

        // boost::shared_ptr<TaskBase> AddTask(boost::shared_ptr<TaskBase> task, long priority);

        template<typename Func>
        boost::shared_ptr<TaskBase> AddTask(Func f);

        template<typename Func>
        boost::shared_ptr<TaskBase> AddTask(Func f, long priority);


        // template <typename T, typename Func>
        // typename FutureTask<T>::Ptr AddFutureTask(Func f);

        void ShutDown();


        void ShutDownNow();


        bool IsShutDown() const;


        //RunMode getRunMode() const;
        //void StopNow();

    private:

        void ThreadFunction(const Function &checkFunc);

        boost::shared_ptr<TaskBase>  DoAddTask(boost::shared_ptr<PriorityTask> task);

        bool Empty();

        boost::shared_ptr<TaskBase> PushTask(boost::shared_ptr<PriorityTask> task);

        boost::shared_ptr<TaskBase> GetTask();

        bool CheckIsRequestShutDown() const;

        void NotifyWhenThreadStop(int threadId);

        bool DoIsShutDown() const;

        void SetState(const State state);

        void DoSetState(const State state);

        static bool CompareFuncGreater(boost::shared_ptr<PriorityTask> left, boost::shared_ptr<PriorityTask> right);

        static bool CompareFuncLesser(boost::shared_ptr<PriorityTask> left, boost::shared_ptr<PriorityTask> right);


        State m_state;

        const RunMode m_runMode;

        // requestShutDown
        std::atomic_bool m_isRequestShutDown;

        // sync tool
        mutable Mutex m_stateGuard;

        ConditionVariable m_stateCond;

        Mutex m_mutex;

        ConditionVariable m_mutexCond;


        typedef bool (*CompareFunc)(boost::shared_ptr<PriorityTask>, boost::shared_ptr<PriorityTask>);

        typedef std::priority_queue<boost::shared_ptr<PriorityTask>,
                std::vector<boost::shared_ptr<PriorityTask> >, CompareFunc> PriorityQueueGreater;

        typedef std::priority_queue<boost::shared_ptr<PriorityTask>,
                std::vector<boost::shared_ptr<PriorityTask> >, CompareFunc> PriorityQueueLesser;


        // FIFO, PRIORITY taskQueue
        PriorityQueueGreater m_taskQueueGreater;

        // LIFO
        PriorityQueueLesser  m_taskQueueLesser;


        boost::scoped_ptr<CloseableThread> m_thread; // Thread must be the last variable

        //Mutex m_handlerGuard;

    };


    template<typename Func>
    boost::shared_ptr<TaskBase> SingleThreadPool::AddTask(Func f)
    {
        return DoAddTask(MakePriorityFunctorTask(f, timeInSeconds()));
    }

    template<typename Func>
    boost::shared_ptr<TaskBase> SingleThreadPool::AddTask(Func f, long priority)
    {
        return DoAddTask(MakePriorityFunctorTask(f, priority));
    }


} //namespace empool

#endif
