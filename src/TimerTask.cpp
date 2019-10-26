#include "TimerTask.h"

#include <boost/bind.hpp>

#include <cassert>
#include <iostream>

#include <sys/time.h>

using namespace std;
namespace zhanmm {


// TimeValue GetCurrentTime()
// {
//     struct timeval now;
//     gettimeofday(&now, NULL);
//     TimeValue now_in_ms = now.tv_sec;
//     now_in_ms = now_in_ms * 1000 + now.tv_usec / 1000;
//     return now_in_ms;
// }

TimerTask::TimerTask()
: m_deadline(0), m_interval(0)
{}

TimeValue TimerTask::GetDeadline() const
{
    MutexLocker lock(m_time_guard);
    return m_deadline;
}

void TimerTask::SetDeadline(const TimeValue deadline)
{
    MutexLocker lock(m_time_guard);
    m_deadline = deadline;
}

bool TimerTask::IsIntervalTask() const
{
    MutexLocker lock(m_time_guard);
    return m_interval > 0;
}

TimeValue TimerTask::GetInterval() const
{
    MutexLocker lock(m_time_guard);
    return m_interval;
}

void TimerTask::SetInterval(const TimeValue interval)
{
    MutexLocker lock(m_time_guard);
    m_interval = interval;
}


} //namespace zhanmm

