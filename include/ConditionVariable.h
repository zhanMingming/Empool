#ifndef ZHANMM_VARIABLE_H_
#define ZHANMM_VARIABLE_H_

#include "Mutex.h"
#include <pthread.h>
#include <boost/noncopyable.hpp>

namespace zhanmm {

typedef unsigned long long TimeValue;


class ConditionVariable : private boost::noncopyable {

public:
    explicit ConditionVariable(Mutex& m);
    ~ConditionVariable();

    /// NOTE: Normally you should not use the following
    /// functions directly. Consider use ConditionWaitLocker
    /// and ConditionNotifyLocker first.
    void Notify();
    void NotifyAll();

    void Wait();

    /// returns true when the condition is notified,
    /// otherwise return false on time expired
    bool TimeWait(TimeValue delay_in_ms);

    void Lock();
    void Unlock();

    Mutex& m_mutex;
    pthread_cond_t m_cond;
};


class ConditionWaitLocker : private boost::noncopyable {
public:
    /// This ctor is usually used with TimedWait.
    /// If you don't use TimedWait, DO NOT use this ctor.
    explicit ConditionWaitLocker(ConditionVariable& c)
    : m_conditionVariable(c)
    {}

    template<typename WaitConditionFunc>
    ConditionWaitLocker(ConditionVariable& c, WaitConditionFunc f)
    : m_conditionVariable(c)
    {
        m_conditionVariable.Lock();
        while (f())
        {
            m_conditionVariable.Wait();
        }
    }

    /// returns *true* when the condition is notified and
    /// function f returns true, otherwise returns *false*
    /// when time expired.
    /// Note that when the function returns, the mutex is locked.
    template<typename WaitConditionFunc>
    bool TimedWait(WaitConditionFunc f, TimeValue delay_in_ms)
    {
        m_conditionVariable.Lock();
        while (f())
        {
            if (!m_conditionVariable.TimeWait(delay_in_ms))
            {
                return false;
            }
        }
        return true;
    }

    ~ConditionWaitLocker()
    {
        m_conditionVariable.Unlock();
    }

private:
    ConditionVariable& m_conditionVariable;
};


class ConditionNotifyLocker : private boost::noncopyable {
public:
    template<typename NotifyConditionFunc>
    explicit ConditionNotifyLocker(ConditionVariable& c,
            NotifyConditionFunc f)
    : m_conditionVariable(c)
    {
        m_conditionVariable.Lock();
        if (f())
        {
            // TODO: change to notify after unlock
            m_conditionVariable.Notify();
        }
        // The user code should set the condition to true
    }

    explicit ConditionNotifyLocker(ConditionVariable& c)
    : m_conditionVariable(c)
    {
        m_conditionVariable.Lock();
        // TODO: change to notify after unlock
        m_conditionVariable.Notify();
    }

    ~ConditionNotifyLocker()
    {
        m_conditionVariable.Unlock();
    }

private:
    ConditionVariable& m_conditionVariable;
};


class ConditionNotifyAllLocker : private boost::noncopyable {
public:
    template<typename NotifyConditionFunc>
    explicit ConditionNotifyAllLocker(ConditionVariable& c,
            NotifyConditionFunc f)
    : m_conditionVariable(c)
    {
        m_conditionVariable.Lock();
        if (f())
        {
            m_conditionVariable.NotifyAll();
        }
    }

    ~ConditionNotifyAllLocker()
    {
        m_conditionVariable.Unlock();
    }

private:
    ConditionVariable& m_conditionVariable;
};

}  //zhanmm

#endif
