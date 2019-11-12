#include "CyclicBarrier.h"
#include "Mutex.h"

using namespace std;
using namespace empool;

CyclicBarrier::CyclicBarrier(const int count)
    : m_max_count(count),
      m_count(count),
      m_reset_index(0),
      m_barrier_cond(0),
      m_mutex(),
      m_condition_variable(m_mutex)
{
    assert(m_count > 0);
}


int CyclicBarrier::Await()
{

    MutexLocker lock(m_mutex);
    --m_count;

    if (m_count == 0)
    {
        m_condition_variable.NotifyAll();
        m_count = m_max_count;
        ++m_barrier_cond;

    }
    else
    {
        const int current_reset_index = m_reset_index;
        const int current_barrier_cond = m_barrier_cond;

        while(current_barrier_cond == m_barrier_cond && current_reset_index == m_reset_index)
        {
            m_condition_variable.Wait();
        }

        if (current_reset_index != m_reset_index)
        {
            return BROKEN;
        }
    }
    return SUCCESS;
}

int CyclicBarrier::Await(int  time_ms)
{
    MutexLocker lock(m_mutex);
    --m_count;

    if (m_count == 0)
    {
        m_condition_variable.NotifyAll();
        m_count = m_max_count;
        ++m_barrier_cond;

    }
    else
    {
        const int current_reset_index = m_reset_index;
        const int current_barrier_cond = m_barrier_cond;

        while(current_barrier_cond == m_barrier_cond && current_reset_index == m_reset_index)
        {
            if(!m_condition_variable.TimeWait(time_ms))
            {
                return TIMEOUT;
            }
        }

        if (current_reset_index != m_reset_index)
        {
            return BROKEN;
        }
    }
    return SUCCESS;

}


void CyclicBarrier::Reset()
{
    MutexLocker lock(m_mutex);
    m_condition_variable.NotifyAll();
    m_count = m_max_count;
    ++m_reset_index;
}


