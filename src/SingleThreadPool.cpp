#include "SingleThreadPool.h"
#include <iostream>

namespace zhanmm
{


    // Implementation
    //  RUN_MODE:
    //  FIFO, LIFO, priority

    SingleThreadPool::SingleThreadPool(RunMode mode)
        : m_state(INIT),
          m_runMode(mode),
          m_isRequestShutDown(false),
          m_stateGuard(),
          m_stateCond(m_stateGuard),
          m_mutex(),
          m_mutexCond(m_mutex),
          m_taskQueueGreater(SingleThreadPool::CompareFuncGreater),
          m_taskQueueLesser(SingleThreadPool::CompareFuncLesser)
    {

        m_thread.reset(new CloseableThread(
                           boost::bind(&SingleThreadPool::ThreadFunction, this, _1), boost::protect(boost::bind(&SingleThreadPool::NotifyWhenThreadStop, this, _1))));


        SetState(RUNNING);
    }


    SingleThreadPool::~SingleThreadPool()
    {
        // keep other thread from pushing more tasks
        ShutDown();
    }


    size_t SingleThreadPool::GetThreadNum() const
    {
        return 1;
    }


    bool  SingleThreadPool::CompareFuncGreater(boost::shared_ptr<PriorityTask> left, boost::shared_ptr<PriorityTask> right)
    {
        return left->GetPriorityValue() > right->GetPriorityValue();
    }

    bool SingleThreadPool::CompareFuncLesser(boost::shared_ptr<PriorityTask> left, boost::shared_ptr<PriorityTask> right)
    {
        return !CompareFuncGreater(left, right);
    }



    boost::shared_ptr<TaskBase>  SingleThreadPool::DoAddTask(boost::shared_ptr<PriorityTask> task)
    {
        if (m_isRequestShutDown.load())
        {
            return boost::shared_ptr<TaskBase>();
        }
        return PushTask(task);
    }

    boost::shared_ptr<TaskBase> SingleThreadPool::PushTask(boost::shared_ptr<PriorityTask> task)
    {

        ConditionNotifyLocker lock(m_mutexCond, boost::bind(&SingleThreadPool::Empty, this));

        m_runMode == LIFO ? m_taskQueueLesser.push(task) : m_taskQueueGreater.push(task);
        return task;

    }


    bool SingleThreadPool::Empty()
    {
        return  m_runMode == LIFO ? m_taskQueueLesser.empty() : m_taskQueueGreater.empty();
    }


    boost::shared_ptr<TaskBase> SingleThreadPool::GetTask()
    {

        ConditionWaitLocker lock(m_mutexCond, boost::bind(&SingleThreadPool::Empty, this));
        boost::shared_ptr<TaskBase> task =  m_runMode == LIFO ? m_taskQueueLesser.top() : m_taskQueueGreater.top();
        m_runMode == LIFO ? m_taskQueueLesser.pop() : m_taskQueueGreater.pop();
        return task;
    }

    void SingleThreadPool::ThreadFunction(const Function &checkFunc)
    {
        while(true)
        {

            checkFunc();

            boost::shared_ptr<TaskBase> runTask = GetTask();

            if (dynamic_cast<EndPriorityTask*>(runTask.get()) != NULL)
            {
                break; // stop thread
            } else {
                runTask->Run();
            }
        }
    }


    bool SingleThreadPool::IsShutDown() const
    {
        MutexLocker lock(m_stateGuard);
        return DoIsShutDown();
    }


    bool SingleThreadPool::DoIsShutDown() const
    {
        return m_state == SHUTDOWN;
    }


    bool SingleThreadPool::CheckIsRequestShutDown() const
    {
        return m_isRequestShutDown.load();
    }


    void SingleThreadPool::ShutDownNow()
    {
        using boost::bind;
        using std::mem_fun;
        using std::not1;

        ShutDown();
        ConditionWaitLocker l(m_stateCond,
                              bind(not1(mem_fun(&SingleThreadPool::
                                                DoIsShutDown)),
                                   this));
    }


    void SingleThreadPool::ShutDown()
    {

        bool flag = false;
        if (m_isRequestShutDown.compare_exchange_weak(flag, true, std::memory_order_release, std::memory_order_relaxed))
        {

            SetState(STOP);

            m_thread->AsyncClose();

            boost::shared_ptr<PriorityTask> endTask(new EndPriorityTask());
            PushTask(endTask);

        }
    }

    void SingleThreadPool::NotifyWhenThreadStop(int threadId)
    {
        SetState(SHUTDOWN);
        ConditionNotifyLocker lock(m_stateCond, boost::bind(&SingleThreadPool::CheckIsRequestShutDown, this));
    }



    void SingleThreadPool::SetState(const State state)
    {
        MutexLocker l(m_stateGuard);
        DoSetState(state);
    }


    void SingleThreadPool::DoSetState(const State state)
    {
        m_state = state;
    }



} //namespace zhanmm
