#include "FixedThreadPool.h"
#include ""





namespace zhanmm {



class ThreadPoolManager {
public:
    //创建一个可重用固定线程数的线程池，以共享的无界队列方式来运行这些线程。
    static FixedThreadPool* newFixedThreadPool(int num) {
        
    }
    //动态线程池
    //static newCachedThreadPool();



    //newScheduledThreadPool创建一个定长线程池，支持定时及周期性任务执行
    static ScheduledThreadPool* newScheduledThreadPool() {
    
    }

    // 创建一个单线程化的线程池，它只会用唯一的工作线程来执行任务，
    // 保证所有任务按照指定顺序(FIFO, LIFO, 优先级)执行
    static   newSingleThreadExecutor {
    

    }



}


} //namespace zhanmm