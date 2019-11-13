#include <empool/ThreadPoolManager.h>
#include <empool/Mutex.h>
#include <empool/TaskBase.h>
#include <atomic>
#include <iostream>
#include <boost/shared_ptr.hpp>

using namespace empool;
using namespace std;

Mutex m_mutex;
std::atomic<int>  a(0);

void func() {
    MutexLocker l(m_mutex);
    ++a;
}


int main(void) {
    boost::shared_ptr<FixedThreadPool> fixedThreadPool(ThreadPoolManager::newFixedThreadPool());
    sleep(1);
    std::cout << fixedThreadPool->GetThreadNum() << std::endl;
    std::cout << a.load() << std::endl;
    for(int index = 0; index <= 100000; ++index) {
        fixedThreadPool->AddTask(func);
    }
    std::cout << fixedThreadPool->GetThreadNum() << std::endl;

    std::cout << a.load() << std::endl;

}



