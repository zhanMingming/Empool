
#ifndef ZHANMM_WORKER_THREAD_H_
#define ZHANMM_WORKER_THREAD_H_

#include "TaskQueueBase.h"
#include "ConditionVariable.h"
#include "CloseableThread.h"

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <memory>
#include <exception>

namespace zhanmm {

const int MAX_WAIT_IN_MS = 1000 * 30;

class WorkerThread : public boost::noncopyable {
private:
    enum State {
        INIT,
        RUNNING,
        FINISHED,
    };

public:
    //typedef boost::shared_ptr<WorkerThread> Ptr;
    typedef CloseableThread::Function Function;

    WorkerThread(boost::shared_ptr<TaskQueueBase> taskQueue, bool isScaling = false);
    WorkerThread(boost::shared_ptr<TaskQueueBase> taskQueue, const Function& action, bool isScaling = false);
    ~WorkerThread();

    void Close();
    void AsyncClose();
private:
    void Init(boost::shared_ptr<TaskQueueBase> taskQueue, const Function& action);
    void ProcessError(const std::exception& e);
    void WorkFunction(const Function& checkFunc);
    void GetTask();
    bool GetTask(boost::shared_ptr<TaskBase>& task, int wait_in_ms);

    bool m_isScaling;
    boost::shared_ptr<TaskQueueBase> m_taskQueue;
    boost::shared_ptr<TaskBase>  m_runningTask;
    mutable Mutex m_runningTaskGuard;
    boost::scoped_ptr<CloseableThread> m_thread; // Thread must be the last variable
    
};


}  // namespace tpool

#endif
