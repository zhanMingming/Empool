#ifndef ZHANMM_MUTEX_H_
#define ZHANMM_MUTEX_H_

#include <pthread.h>
#include <boost/noncopyable.hpp>

#include <cassert>


namespace empool
{


    class Mutex : private boost::noncopyable
    {

    public:
        Mutex();
        ~Mutex();

        // These two functions can only called by MutexLocker
        void Lock();
        void Unlock();
        bool TryLock();
        void Init();
        pthread_mutex_t *getMutexPtr();

    private:
        pthread_mutex_t m_mutex;
        volatile bool m_isInit;
    };


    class MutexLocker : private boost::noncopyable
    {
    public:
        explicit MutexLocker(Mutex &m);
        ~MutexLocker();

    private:
        Mutex &m_mutex;
    };


}  // namespace empool

#endif
