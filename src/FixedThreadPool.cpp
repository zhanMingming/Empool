#include "FixedThreadPool.h"

namespace empool {

FixedThreadPool::FixedThreadPool(const size_t threadNum)
:scalingThreadPool(threadNum, threadNum)
{

}

FixedThreadPool::~FixedThreadPool() 
{}

size_t FixedThreadPool::GetThreadNum() const 
{
    return scalingThreadPool.GetThreadNum();
}


boost::shared_ptr<TaskBase> FixedThreadPool::AddTask(boost::shared_ptr<TaskBase> task) 
{
    return scalingThreadPool.AddTask(task);
}



void FixedThreadPool::ShutDown() 
{
    scalingThreadPool.ShutDown();
}

void FixedThreadPool::ShutDownNow() 
{
    scalingThreadPool.ShutDownNow();
}


bool FixedThreadPool::IsShutDown() const 
{
    return scalingThreadPool.IsShutDown();
}



} //namespace empool