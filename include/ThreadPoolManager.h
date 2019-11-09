#ifndef  ZHANMM_THREAD_POOL_MANAGER_H
#define  ZHANMM_THREAD_POOL_MANAGER_H

#include "FixedThreadPool.h"
#include "ScheduledThreadPool.h"
#include "ScalingThreadPool.h"
#include "SingleThreadPool.h"



namespace zhanmm
{



    class ThreadPoolManager
    {
    public:
        //创建一个可重用固定线程数的线程池，以共享的无界队列方式来运行这些线程。
        static FixedThreadPool *newFixedThreadPool(int threadNum = 4)
        {
            return new FixedThreadPool(threadNum);
        }
        //动态线程池
        static ScalingThreadPool *newScalingThreadPool(int minThreadSize = 4, int maxThreadSize = 8)
        {
            return new ScalingThreadPool(minThreadSize, maxThreadSize);
        }


        //newScheduledThreadPool创建一个定长线程池，支持定时及周期性任务执行
        static ScheduledThreadPool *newScheduledThreadPool(int threadNum = 1)
        {
            return new ScheduledThreadPool(threadNum);
        }


        // 创建一个单线程化的线程池，它只会用唯一的工作线程来执行任务，
        // 保证所有任务按照指定顺序(FIFO, LIFO, 优先级)执行
        static   SingleThreadPool* newSingleThreadPool(RunMode mode) 
        {
            return new SingleThreadPool(mode);
        }


    };


} //namespace zhanmm

#endif
