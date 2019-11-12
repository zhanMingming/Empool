#include "TaskBase.h"
#include "Mutex.h"
#include <exception>
#include <string>
#include <functional>
#include <boost/bind.hpp>

using namespace std;
namespace empool
{

    namespace
    {
        class TaskCancelException : public exception
        {
        public:
            explicit TaskCancelException(const string &s)
                : m_cancelMessage(s)
            {}

            virtual ~TaskCancelException() throw()
            {}

            virtual const char *what() const throw()
            {
                return m_cancelMessage.c_str();
            }

        private:
            string m_cancelMessage;
        };
    }

    TaskBase::TaskBase()
        : m_state(INIT), m_stateGuard(), m_isRequestCancel(false), m_cancelCondition(m_stateGuard)
    {

    }

    void TaskBase::Run()
    {
        try
        {
            CheckCancellation(); // check before running the task.

            SetState(RUNNING);
            DoRun();
            SetState(FINISHED);
        }
        catch (const TaskCancelException &)
        {
            SetState(CANCELLED);
            //OnCancel();
        }

        // wake up the waiting threads that is cancelling this task.
        ConditionNotifyAllLocker(m_cancelCondition,
                                 boost::bind(&TaskBase::IsRequestCancel, this));
    }

    void TaskBase::Cancel()
    {

        m_isRequestCancel.store(true);
        // wait until the task is cancelled.
        ConditionWaitLocker(m_cancelCondition,
                            boost::bind(not1(mem_fun(&TaskBase::IsStopState)),
                                        this));
    }

    // void TaskBase::CancelAsync()
    // {
    //   m_isRequestCancel = true;
    //   OnCallCancel();
    // }

    TaskBase::State TaskBase::GetState() const
    {
        MutexLocker lock(m_stateGuard);
        return m_state;
    }

    bool TaskBase::IsRunning() const
    {
        return GetState() == RUNNING;
    }

    bool TaskBase::IsFinished() const
    {
        return GetState() == FINISHED;
    }

    bool TaskBase::IsCancelled() const
    {
        return GetState() == CANCELLED;
    }

    // "Stop" means state == FINISHED or state == CANCELLED
    bool TaskBase::IsStopped() const
    {
        // Cannot use GetState() here because we must ensure
        // this whole function is atomic. If we use GetState(),
        // extra lock will cause deadlock.
        MutexLocker lock(m_stateGuard);
        return IsStopState();
    }

    void TaskBase::CheckCancellation() const
    {
        if (m_isRequestCancel.load())
        {
            throw TaskCancelException("cancel task");
        }
    }

    // inline void TaskBase::OnCancel()
    // {
    //     // The default cancal action is doing nothing.
    // }

    // inline void TaskBase::OnCallCancel()
    // {
    //     // The default cancal action is doing nothing.
    // }

    void TaskBase::SetState(const State state)
    {
        MutexLocker lock(m_stateGuard);
        m_state = state;
    }

    bool TaskBase::IsRequestCancel() const
    {
        return m_isRequestCancel.load();
    }

    bool TaskBase::IsStopState() const
    {
        return m_state == FINISHED || m_state == CANCELLED;
    }

} //namespace empool
