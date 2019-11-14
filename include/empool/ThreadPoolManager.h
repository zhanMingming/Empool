#ifndef  ZHANMM_THREAD_POOL_MANAGER_H
#define  ZHANMM_THREAD_POOL_MANAGER_H

#include "FixedThreadPool.h"
#include "ScheduledThreadPool.h"
#include "ScalingThreadPool.h"
#include "SingleThreadPool.h"

namespace empool
{

    class ThreadPoolManager
    {
    public:
        //Create a pool of threads that can reuse a fixed number of threads and run them in a shared, unbounded queue.
        static FixedThreadPool *newFixedThreadPool(int threadNum = 4)
        {
            return new FixedThreadPool(threadNum);
        }
        //ScalingThreadPool 
        static ScalingThreadPool *newScalingThreadPool(int minThreadSize = 4, int maxThreadSize = 8)
        {
            return new ScalingThreadPool(minThreadSize, maxThreadSize);
        }


        //Create a fixed length thread pool to support scheduled and periodic task execution
        static ScheduledThreadPool *newScheduledThreadPool(int threadNum = 1)
        {
            return new ScheduledThreadPool(threadNum);
        }


        // Create a single threaded thread pool that only uses unique worker threads to perform tasks
        // Ensure that all tasks are executed in the specified order (FIFO, LIFO, PRIORITY)
        static   SingleThreadPool* newSingleThreadPool(RunMode mode) 
        {
            return new SingleThreadPool(mode);
        }
    };

} //namespace empool

#endif
