
#include "CloseableThread.h"

#include <boost/bind.hpp>

#include <cassert>
#include <string>
#include <iostream>


using namespace std;
using namespace boost;

namespace empool
{

    namespace
    {
        // This exception makes thread exit.
        class CloseableThreadExitException : public std::exception
        {
        public:
            explicit CloseableThreadExitException(const string &s)
                : m_exitMessage(s)
            {}

            virtual ~CloseableThreadExitException() throw()
            {}

            virtual const char *what() const throw()
            {
                return m_exitMessage.c_str();
            }

        private:
            string m_exitMessage;
        };
    }  // namespace


    CloseableThread::CloseableThread(WorkFunction workFunction)
        :m_state(INIT), m_mutex(), m_stateGuard(m_mutex), m_isRequestClose(false), m_workFunction(workFunction)
    {
        Init();
    }

    CloseableThread::CloseableThread(
        WorkFunction workFunction, FinishAction finishAction)
        :  m_state(INIT), m_mutex(), m_stateGuard(m_mutex), m_isRequestClose(false), m_workFunction(workFunction),
          m_finishAction(finishAction)
    {
        std::cout << "start CloseableThread" << std::endl;
        Init();
    }

    CloseableThread::~CloseableThread()
    {
    }


    int CloseableThread::GetThreadId() const
    {
        return m_thread->GetThreadId();
    }

    void CloseableThread::AsyncClose()
    {
        bool flag = false;
        m_isRequestClose.compare_exchange_weak(flag, true, std::memory_order_release, std::memory_order_relaxed);

    }


    void CloseableThread::Close()
    {
        bool flag = false;
        if (m_isRequestClose.compare_exchange_weak(flag, true, std::memory_order_release, std::memory_order_relaxed))
        {
            ConditionWaitLocker l(m_stateGuard,
                                  BOOST_BIND(not1(mem_fun(&CloseableThread::IsFinished)),
                                             this));
        }
    }


    bool CloseableThread::IsRequestClose() const
    {
        return m_isRequestClose.load();
    }

    void CloseableThread::Init()
    {

        assert(m_workFunction);
        // ensure that the thread is created successfully.
        while (true)
        {
            try
            {
                // check for the creation exception
                m_thread.reset(new Thread(BOOST_BIND(
                                              &CloseableThread::ThreadFunction, this)));
                break;
            }
            catch (const std::exception &e)
            {
                std::cout << "thread_create error" << std::endl;
                ProcessError(e);
            }
        }
    }

    void CloseableThread::CheckIsRequestClose() const
    {
        if (m_isRequestClose.load())
        {
            throw CloseableThreadExitException("Thread Closed");
        }
    }

    void CloseableThread::ProcessError(const std::exception &e)
    {
        cerr << "CloseableThread actor" << endl;
        cerr << e.what() << endl;
        cerr << "Try again." << endl;
    }

    // bool CloseableThread::IsFinished() const
    // {
    //     sync::MutexLocker l(m_mutex);
    //     return IsFinished();
    // }



    bool CloseableThread::IsFinished() const
    {
        //MutexLocker lock(m_mutex);
        return m_state == FINISHED;
    }


    // void CloseableThread::SetState(const State state)
    // {
    //     sync::MutexLocker l(m_stateGuard);
    //     DoSetState(state);
    // }

    void CloseableThread::SetState(const State state)
    {
        //std::cout << "set state" << std::endl;
        //MutexLocker lock(m_mutex);
        //std::cout << "alredy set state" << std::endl;
        m_state = state;
    }

    void CloseableThread::NotifyFinished()
    {
        SetState(FINISHED);
        std::cout << "CloseableThread : set FINISHED" << std::endl;
        ConditionNotifyAllLocker l(m_stateGuard,
                                   BOOST_BIND(&CloseableThread::IsRequestClose, this));
    }

    void CloseableThread::ThreadFunction()
    {
        SetState(RUNNING);
        try
        {
            CheckIsRequestClose();
            m_workFunction(
                BOOST_BIND(&CloseableThread::CheckIsRequestClose, this));
        }
        catch (const CloseableThreadExitException &)
        {
            cerr << " Success Close Thread id: " + to_string(m_thread->GetThreadId()) << endl;
        }
        // catch (...) // caught other exception
        // {
        //     cerr << "other exception occur" << endl;
        // }

        if (m_finishAction)
        {
            m_finishAction(GetThreadId());
        }
        NotifyFinished();
    }

} //namespace empool