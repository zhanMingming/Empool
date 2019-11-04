#ifndef ZHANMM_CLOSEABLE_THREAD_H_
#define ZHANMM_CLOSEABLE_THREAD_H_

#include "Thread.h"
#include "ConditionVariable.h"
#include "Mutex.h"
#include <atomic>
#include <memory>
#include <exception>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace zhanmm {

  // This class can not be derived.
class CloseableThread : private ::boost::noncopyable {
public:
    typedef boost::function<void(int)> FinishAction;
    typedef boost::function<void()> Function;
    typedef boost::function<void(Function)> WorkFunction;

    CloseableThread(WorkFunction workFunction);
    CloseableThread(WorkFunction workFunction, FinishAction finishAction);
    ~CloseableThread();

    void Close();
    void AsyncClose();
    bool IsRequestClose() const;
    int GetThreadId() const;
    //bool IsClose() const;

protected:
    enum State {
      INIT,
      RUNNING,
      FINISHED,
  };

private:
    void Init();
    void CheckIsRequestClose() const;
    void ProcessError(const std::exception& e);
    bool IsFinished() const;
    
    void SetState(const State state);
    // void DoSetState(const State state);
    // void GetTaskFromTaskQueue();
    void NotifyFinished();
    void ThreadFunction();

private:
    State m_state;
    mutable Mutex m_mutex;
    mutable ConditionVariable m_stateGuard;
    std::atomic_bool m_isRequestClose;
    WorkFunction m_workFunction;
    FinishAction m_finishAction;
    boost::scoped_ptr<Thread> m_thread; // Thread must be the last variable
};




}  // namespace zhanmm

#endif  