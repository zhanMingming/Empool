#ifndef ZHANMM_FUTURE_TASK_H_
#define ZHANMM_FUTURE_TASK_H_

#include "TaskBase.h"
#include "ConditionVariable.h"
#include "Mutex.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <functional>

namespace empool {
  class ResultNotReadyException
  {
  };
  
  template <typename T>
  class FutureTask : public TaskBase {
  public:
    typedef boost::shared_ptr<FutureTask> Ptr;
    typedef T ReturnType;
    
    FutureTask();
    virtual ~FutureTask();

    T GetResult();
    T TryGetResult() throw(ResultNotReadyException);
    
  private:
    virtual void DoRun();
    virtual T Call() = 0;
    bool IsResultReturned() const;

    T m_result;
    bool m_isResultReturned;
    Mutex m_mutex;
    ConditionVariable m_resultCondition;
  };

  // Partial specialization for void, which forbids
  // users from defining a method returning void.
  template <>
  class FutureTask<void>;


  // Implementation
  template <typename T>
  FutureTask<T>::FutureTask()
    : m_isResultReturned(false), m_mutex(), m_resultCondition(m_mutex)
  {}

  template <typename T>
  FutureTask<T>::~FutureTask()
  {}

  template <typename T>
  T FutureTask<T>::GetResult()
  {
    using std::not1;
    using std::mem_fun;
    using boost::bind;
    
    ConditionWaitLocker l(m_resultCondition,
                bind(not1(mem_fun(&FutureTask::IsResultReturned)),
                     this));
    return m_result;
  }

  template <typename T>
  void FutureTask<T>::DoRun()
  {
    using std::not1;
    using std::mem_fun;
    using boost::bind;

    ConditionNotifyAllLocker l(m_resultCondition,
                     bind(not1(mem_fun(&FutureTask::IsResultReturned)),
                      this));
    m_result = Call();
    m_isResultReturned = true;
  }

  template <typename T>
  bool FutureTask<T>::IsResultReturned() const
  {
    return m_isResultReturned;
  }

  template <typename T>
  T FutureTask<T>::TryGetResult()
    throw(ResultNotReadyException)
  {
    MutexLocker l(m_resultCondition);
    if (!m_isResultReturned)
      {
    throw ResultNotReadyException();
      }
    return m_result;
  }
}

#endif
