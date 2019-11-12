
#ifndef ZHANMM_FIXED_THREAD_POOL_H_
#define ZHANMM_FIXED_THREAD_POOL_H_

#include "WorkerThread.h"
#include "BlockingTaskQueue.h"
#include "FunctorTask.h"
#include "EndTask.h"
#include "ConditionVariable.h"
#include "ScalingThreadPool.h"

#include <atomic>
#include <vector>
#include <cstdlib> // for size_t
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <functional>
#include <iostream>


namespace empool {
  

class FixedThreadPool : public boost::noncopyable {
  public:
    FixedThreadPool(const size_t threadNum = 8);
    ~FixedThreadPool();
  

    size_t GetThreadNum() const;

    boost::shared_ptr<TaskBase> AddTask(boost::shared_ptr<TaskBase> task);

    template<typename Func>
    boost::shared_ptr<TaskBase> AddTask(Func f);


    // template <typename T, typename Func>
    // typename FutureTask<T>::Ptr AddFutureTask(Func f);

    void ShutDown();


    void ShutDownNow();
    //void StopNow();
    
    bool IsShutDown() const;    
  
  private:
    ScalingThreadPool  scalingThreadPool;
};

template<typename Func>
boost::shared_ptr<TaskBase> FixedThreadPool::AddTask(Func f)
{
    return scalingThreadPool.AddTask(MakeFunctorTask(f));
}


} //namespace empool


#endif
