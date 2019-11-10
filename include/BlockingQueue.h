#ifndef ZHANMM_BLOCKINGQUEUE_H_
#define ZHANMM_BLOCKINGQUEUE_H_

#include "ConditionVariable.h"
#include "Mutex.h"
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <queue>

namespace zhanmm {

template<typename T, typename QueueImpl = std::queue<T> >
class BlockingQueue : private ::boost::noncopyable {
private:
    typedef typename QueueImpl::container_type QueueContainer;

public:
    typedef T ElemType;
    typedef QueueImpl QueueImplType;

    BlockingQueue() :m_mutex(), m_mutexCond(m_mutex){}

    explicit BlockingQueue(const QueueImpl& container)
    :m_mutex(), m_mutexCond(m_mutex), m_queue(container)
    {}

    explicit BlockingQueue(const QueueContainer& container)
    :m_mutex(), m_mutexCond(m_mutex), m_queue(container)
    {}


    void Push(const ElemType& elem)
    {
        ConditionNotifyAllLocker l(m_mutexCond,
                boost::bind(&QueueImpl::empty, &m_queue));
        m_queue.push(elem);
    }

    // Return copy here to avoid concurrent acess issue
    ElemType Pop()
    {
        // wait until task queue is not empty
        ConditionWaitLocker l(m_mutexCond,
                boost::bind(&QueueImpl::empty, &m_queue));

        ElemType elem = m_queue.front();
        m_queue.pop();
        return elem;
    }

    // Use this version of Pop if copying object is expensive
    void Pop(ElemType& elem)
    {
        // wait until task queue is not empty
        ConditionWaitLocker l(m_mutexCond,
                boost::bind(&QueueImpl::empty, &m_queue));

        elem = m_queue.front();
        m_queue.pop();
    }

    bool PopTimeWait(ElemType& elem, int time_in_ms) {
        
        ConditionWaitLocker l(m_mutexCond);
        if (!l.TimeWait(boost::bind(&QueueImpl::empty, &m_queue), time_in_ms))  {
            return false;
        }
        elem = m_queue.front();
        m_queue.pop();
        return true;
    }


    ElemType Front() const
    {
        // wait until task queue is not empty
        ConditionWaitLocker l(m_mutexCond,
                boost::bind(&QueueImpl::empty, &m_queue));
        return m_queue.front();
    }

    // Use this version of Front if copying object is expensive
    void Front(ElemType& elem) const
    {
        // wait until task queue is not empty
        ConditionWaitLocker l(m_mutexCond,
                boost::bind(&QueueImpl::empty, &m_queue));

        elem = m_queue.front();
    }


    size_t Size() const
    {
        MutexLocker lock(m_mutex);
        return m_queue.size();
    }

    bool IsEmpty() const
    {
        MutexLocker lock(m_mutex);
        return m_queue.empty();
    }

private:
    mutable Mutex m_mutex;
    mutable ConditionVariable m_mutexCond;
    QueueImpl m_queue;
};


}  // namespace zhanmm


#endif /* ZHANMM_BLOCKINGQUEUE_H_ */
