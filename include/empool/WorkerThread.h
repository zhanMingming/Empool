#ifndef ZHANMM_WORKER_THREAD_H_
#define ZHANMM_WORKER_THREAD_H_

#include "TaskQueueBase.h"
#include "ConditionVariable.h"
#include "CloseableThread.h"

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <memory>
#include <exception>


namespace empool
{

    const int MAX_WAIT_IN_MS = 1000 * 10;

    //class ScalingThreadPool;

    class WorkerThread : public boost::noncopyable
    {
    private:
        enum State
        {
            INIT,
            RUNNING,
            FINISHED,
        };

    public:
        //typedef boost::shared_ptr<WorkerThread> Ptr;
        typedef CloseableThread::FinishAction FinishAction;
        typedef CloseableThread::Function Function;
        typedef boost::function<bool()> JudgeAction;


        WorkerThread(boost::shared_ptr<TaskQueueBase> taskQueue, const JudgeAction  judge);
        WorkerThread(boost::shared_ptr<TaskQueueBase> taskQueue, const FinishAction &action, const JudgeAction  judge);
        ~WorkerThread();
        int GetThreadId() const;

        void Close();
        void AsyncClose();
    private:
        void Init(boost::shared_ptr<TaskQueueBase> taskQueue, const FinishAction &action);
        void ProcessError(const std::exception &e);
        void WorkFunction(const Function &checkFunc);
        void GetTask();
        bool GetTask(boost::shared_ptr<TaskBase> &task, int wait_in_ms);

        const JudgeAction  m_judge;
        boost::shared_ptr<TaskQueueBase> m_taskQueue;
        boost::shared_ptr<TaskBase>  m_runningTask;
        mutable Mutex m_runningTaskGuard;
        boost::scoped_ptr<CloseableThread> m_thread; // Thread must be the last variable

    };


}  // namespace tpool

#endif
