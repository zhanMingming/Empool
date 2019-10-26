#ifndef ZHANMM_END_TASK_H_
#define ZHANMM_END_TASK_H_

#include "TaskBase.h"

namespace zhanmm {
  class EndTask : public TaskBase {
  public:
    virtual void DoRun();
  };
}

#endif
