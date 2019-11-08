#include "ConditionVariable.h"
#include "Mutex.h"

#include <sys/time.h>
#include <errno.h>
#include <iostream>

using namespace zhanmm;


ConditionVariable::ConditionVariable(Mutex &m)
    : m_mutex(m)
{
    pthread_cond_init(&m_cond, NULL);
}

ConditionVariable::~ConditionVariable()
{
    pthread_cond_destroy(&m_cond);
}

void ConditionVariable::Notify()
{
    pthread_cond_signal(&m_cond);
}

void ConditionVariable::NotifyAll()
{
    pthread_cond_broadcast(&m_cond);
}

void ConditionVariable::Wait()
{
    pthread_cond_wait(&m_cond, m_mutex.getMutexPtr());
}

void ConditionVariable::Lock()
{
    m_mutex.Lock();
}

bool ConditionVariable::TimeWait(TimeValue delay_in_ms)
{
    struct timeval now;
    struct timespec timeout;

    gettimeofday(&now, NULL);
    std::cout << "delay_in_ms: " << static_cast<long>(delay_in_ms / 1000) << std::endl;
    timeout.tv_sec = now.tv_sec + static_cast<long>(delay_in_ms / 1000);
    timeout.tv_nsec = now.tv_usec +
                      static_cast<long>(delay_in_ms % 1000) * 1000;
    if (timeout.tv_nsec >= 1000000)
    {
        ++timeout.tv_sec;
        timeout.tv_nsec = timeout.tv_nsec % 1000000;
    }
    timeout.tv_nsec = timeout.tv_nsec * 1000;


    int ret =  pthread_cond_timedwait(&m_cond, m_mutex.getMutexPtr(), &timeout);
    return 0 == ret;
}

void ConditionVariable::Unlock()
{
    m_mutex.Unlock();
}
