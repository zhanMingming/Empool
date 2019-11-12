
#ifndef ZHANMM_FUNCTOR_TASK_H_
#define ZHANMM_FUNCTOR_TASK_H_

#include "TaskBase.h"


namespace empool {
  template<typename Func>
  class FunctorTask : public TaskBase {
  public:
    FunctorTask(Func f);

    virtual void DoRun();
    
  private:
    Func m_functor;
  };

  /// helper function
  template<typename Func>
  boost::shared_ptr<TaskBase> MakeFunctorTask(Func f)
  {
    return boost::shared_ptr<TaskBase>(new FunctorTask<Func>(f));
  }

  // Implementation
  template<typename Func>
  FunctorTask<Func>::FunctorTask(Func f)
    : m_functor(f)
  {}

  template<typename Func>
  void FunctorTask<Func>::DoRun()
  {
    m_functor();
  }
} // namespace empool

#endif
