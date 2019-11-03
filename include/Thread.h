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
#include <iostream>

namespace zhanmm {
//typedef  unsigned long long  pthread_id;

class Thread : private boost::noncopyable {
public:
    template<class Func>
    explicit Thread(const Func& f);

    ~Thread();

    int GetThreadId() const { 
        return m_threadId;
    }

private:
    template<class Func>
    static void* ThreadFunction(void* arg);

    void ProcessCreateError(const int error);
    static void ProcessException(const std::exception& e);
    static void ProcessUnknownException();

    template<typename Func>
    struct Args {
        Args(int* tid, const Func& f)
        : threadId(tid), func(f)
        {}

        int* threadId;
        Func func;
    };

    pthread_t m_threadData;
    int m_threadId;
    bool m_isStart;
};


// Implementation
template<class Func>
Thread::Thread(const Func& f)
: m_threadId(0), m_isStart(false)
{
    
    std::cout << "thread contructor" << std::endl;
    Args<Func>* args = new Args<Func>(&m_threadId, f);

    int error =  pthread_create(&m_threadData, NULL,
            ThreadFunction<Func>, args);
    if (error != 0)
    {
        std::cout << "thread make fail " << std::endl;
        ProcessCreateError(error);
    }
    m_isStart = true;
}

template<class Func>
void* Thread::ThreadFunction(void* arg)
{
   

    boost::scoped_ptr<Args<Func> > args(reinterpret_cast<Args<Func>*>(arg));
    *(args->threadId) = Tid();
    std::cout << "ThreadFunction" << std::endl;
    std::cout << *(args->threadId) << std::endl;

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
