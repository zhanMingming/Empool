#include "WorkerThread.h"
#include "Thread.h"
#include "TaskQueueBase.h"
#include "TaskBase.h"
#include "EndTask.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <functional>

using namespace std;
//using namespace zhanmm;
using namespace boost;

namespace zhanmm {

namespace {
struct NoOp {
    void operator()(int threadId)
    {}
};
}  // namespace



WorkerThread::WorkerThread(boost::shared_ptr<TaskQueueBase> taskQueue, bool isScaling)
:m_isScaling(isScaling)
{
    Init(taskQueue, NoOp());
}

WorkerThread::WorkerThread(boost::shared_ptr<TaskQueueBase> taskQueue,
        const FinishAction& action, bool isScaling)
:m_isScaling(isScaling)
{
    Init(taskQueue, action);
}

void WorkerThread::Init(boost::shared_ptr<TaskQueueBase> taskQueue,
        const FinishAction& action)
{

    m_taskQueue = taskQueue;

    // ensure that the thread is created successfully.
    while (true)
    {
        try
        {
            // check for the creation exception
            m_thread.reset(new CloseableThread(
                    BOOST_BIND(&WorkerThread::WorkFunction, this, _1), action));
            break;
        }
        catch (const std::exception& e)
        {
            ProcessError(e);
        }
    }
}

// dtor has to be defined for pimpl idiom
WorkerThread::~WorkerThread()
{}


int WorkerThread::GetThreadId() const
{
    return m_thread->GetThreadId();
}

void WorkerThread::Close()
{
    m_thread->Close();
}

void WorkerThread::AsyncClose() 
{
    m_thread->AsyncClose();
}

// void WorkerThread::CancelAsync()
// {
//     m_thread->CancelAsync();
// }

// void WorkerThread::CancelNow()
// {
//     {
//         MutexLocker l(m_runningTaskGuard);
//         if (m_runningTask)
//         {
//             m_runningTask->CancelAsync();
//         }
//     }
//     Cancel();
// }

void WorkerThread::ProcessError(const std::exception& e)
{
    cerr << "WorkerThread ctor" << endl;
    cerr << e.what() << endl;
    cerr << "Try again." << endl;
}

void WorkerThread::WorkFunction(const Function& checkFunc)
{
    while (true)
    {
        // 1. check cancel request
        checkFunc();

        // 2. fetch task from task queue
        if (!m_isScaling) {
            GetTask();
        
        } else if (!GetTask(m_runningTask, MAX_WAIT_IN_MS)) {
        // if can't getTask in wait_in_ms, so AsyncClose()
            AsyncClose();
            checkFunc();
        }

        // 3. perform the task
        if (m_runningTask) {
            if (dynamic_cast<EndTask*>(m_runningTask.get()) != NULL) {
                break; // stop the worker thread.
            } else {
                m_runningTask->Run();
            }
        }
        // 4. perform any post-task action
    }
}

bool WorkerThread::GetTask(boost::shared_ptr<TaskBase>& task, int wait_in_ms) {
    return m_taskQueue->PopTimeWait(task, wait_in_ms);
}


void WorkerThread::GetTask()
{
    //std::cout << "ready get Task" << std::endl;
    //MutexLocker l(m_runningTaskGuard);
    m_runningTask = m_taskQueue->Pop();
}

} //namespace zhanmm
