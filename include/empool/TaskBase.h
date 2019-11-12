#ifndef ZHANMM_TASK_BASE_H_
#define ZHANMM_TASK_BASE_H_

#include "ConditionVariable.h"
#include "Mutex.h"
#include <atomic>
#include <boost/shared_ptr.hpp>

namespace empool
{
    class TaskBase
    {
    public:
        //typedef boost::shared_ptr<TaskBase> Ptr;

        enum State
        {
            INIT,
            RUNNING,
            FINISHED,
            CANCELLED,
        };

        TaskBase();
        virtual ~TaskBase() {}

        void Run();
        void Cancel();
        //void CancelAsync();

        State GetState() const;
        bool IsRunning() const;
        bool IsFinished() const;
        bool IsCancelled() const;
        bool IsRequestCancel() const;
        bool IsStopped() const;

    protected:
        void CheckCancellation() const;
        // virtual void OnCancel();
        // virtual void OnCallCancel();

    private:
        virtual void DoRun() = 0;
        void SetState(const State state);
        bool IsStopState() const;

        State m_state;
        
        mutable Mutex m_stateGuard;

        std::atomic_bool m_isRequestCancel;

        ConditionVariable m_cancelCondition;
    };

} //namespace empool

#endif
