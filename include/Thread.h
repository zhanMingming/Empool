#ifndef ZHANMM_THREAD_H_
#define ZHANMM_THREAD_H_

#include "System.h"

#include <atomic>
#include <pthread.h>
#include <memory>
#include <cstdlib>
#include <stdexcept>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace zhanmm {

class Thread : private boost::noncopyable {
public:
    template<class Func>
    explicit Thread(const Func& f);

    ~Thread();

    int GetThreadId() const { return m_threadId; }

private:
    template<class Func>
    static void* ThreadFunction(void* arg);

    void ProcessCreateError(const int error);
    static void ProcessException(const std::exception& e);
    static void ProcessUnknownException();

    template<typename Func>
    struct Args {
        Args(std::atomic<int>* tid, const Func& f)
        : threadId(tid), func(f)
        {}

        std::atomic<int>* threadId;
        Func func;
    };

    pthread_t m_threadData;
    std::atomic<int> m_threadId;
    bool m_isStart;
};


// Implementation
template<class Func>
Thread::Thread(const Func& f)
: m_threadId(0), m_isStart(false)
{
    

    Args<Func>* args = new Args<Func>(&m_threadId, f);

    int error =  pthread_create(&m_threadData, NULL,
            ThreadFunction<Func>, args);
    if (error != 0)
    {
        ProcessCreateError(error);
    }
    m_isStart = true;
}

template<class Func>
void* Thread::ThreadFunction(void* arg)
{
   

    boost::scoped_ptr<Args<Func> > args(reinterpret_cast<Args<Func>*>(arg));
    *(args->threadId) = Tid();

    try
    {
        (args->func)(); // call the functor
    }
    catch (const std::exception& e)
    {
        ProcessException(e);
    }
    catch (...)
    {
        ProcessUnknownException();
    }

    return NULL;
}

}  

#endif
