#include "ThreadPoolManager.h"

namespace zhanmm {



static FixedThreadPool*  ThreadPoolManager::newFiexedThreadPool(int threadNum) {
    return  new FixedThreadPool(threadNum);
}


static ScheduledThreadPool*  ThreadPoolManager::newScheduledThreadPool(int threadNum) {
    return new ScheduledThreadPool(threadNum);
}


} //namespace zhanmm
