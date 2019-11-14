#include "Thread.h"
#include <iostream>
#include <string>
#include <errno.h>

using namespace std;
namespace empool
{

    void Thread::ProcessCreateError(const int error)
    {
        const string msg = "Create Thread failed: ";
        if (error == EAGAIN) // resource limit
        {
            throw std::runtime_error(msg + "resource limit");
        }
        else if (error == EINVAL)
        {
            throw std::runtime_error(msg + "invalid attribute");
        }
        else if (error == EPERM)
        {
            throw std::runtime_error(msg + "no permission to set scheduling");
        }
        else
        {
            throw std::runtime_error(msg + "unknown error");
        }
    }

    void Thread::ProcessException(const exception &e)
    {
        cerr << e.what() << endl;
        cout << e.what() << endl;
    }

    void Thread::ProcessUnknownException()
    {
        cerr << "Caught Unexpected Excetion." << endl;
    }

    Thread::~Thread()
    {
        if (m_isStart)
        {
            //std::cout << "Thread join : " <<  GetThreadId() << std::endl;
            pthread_join(m_threadData, NULL);
            //std::cout << "Thread join finish" << std::endl;
            m_isStart = false;
        }
    }

} //namespace empool