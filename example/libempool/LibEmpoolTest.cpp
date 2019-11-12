#include <empool/ThreadPoolManager.h>
#include <empool/Mutex.h>
#include <boost/shared_ptr.hpp>
#include <atomic>
#include <empool/ScalingThreadPool.h>


using namespace empool;

Mutex m_mutex;
std::atomic<int>  a(0);

void func()
{
    MutexLocker l(m_mutex);
    MilliSleep(1);
    ++a;
    //std::cout << a << std::endl;
}



int main(void) {

    boost::shared_ptr<ScalingThreadPool> scalingThreadPool(ThreadPoolManager::newScalingThreadPool());

    std::cout << scalingThreadPool->GetThreadNum() << std::endl;
    std::cout << a.load() << std::endl;
    for(int index = 0; index <= 10000; ++index)
    {
        scalingThreadPool->AddTask(func);
    }
    std::cout << scalingThreadPool->GetThreadNum() << std::endl;

    sleep(5);
    std::cout << a.load() << std::endl;
    std::cout << scalingThreadPool->GetThreadNum() << std::endl;
}
