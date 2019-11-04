#include "ThreadPoolManager.h"
#include "Mutex.h"
#include "TaskBase.h"
#include <atomic>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include "Util.h"

using namespace zhanmm;
using namespace std;

Mutex m_mutex;
std::atomic<int>  a(0);

void func() {
    MutexLocker l(m_mutex);
    MilliSleep(1);
    ++a;
    //std::cout << a << std::endl;
}



int main(void) {
    boost::shared_ptr<ScalingThreadPool> scalingThreadPool(ThreadPoolManager::newScalingThreadPool());
    //sleep(1);
    std::cout << scalingThreadPool->GetThreadNum() << std::endl;
    std::cout << a.load() << std::endl;
    for(int index = 0; index <= 10000; ++index) {
        scalingThreadPool->AddTask(func);
    }
    
    std::cout << scalingThreadPool->GetThreadNum() << std::endl;
    //std::cout << scalingThreadPool->GetThreadNum() << std::endl;
    //std::cout << scalingThreadPool->GetThreadNum() << std::endl;

    std::cout << a.load() << std::endl;

    sleep(10);
    std::cout << a.load() << std::endl;
    std::cout << scalingThreadPool->GetThreadNum() << std::endl;


    //sleep(35);
    std::cout << a.load() << std::endl;
    std::cout << scalingThreadPool->GetThreadNum() << std::endl;

}