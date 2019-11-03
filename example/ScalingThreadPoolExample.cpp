/*
* @Author: zhanmingming
* @Date:   2019-11-02 23:46:57
* @Last Modified by:   zhanmingming
* @Last Modified time: 2019-11-03 10:43:06
*/
#include "ThreadPoolManager.h"
#include "Mutex.h"
#include "TaskBase.h"
#include <atomic>
#include <iostream>
#include <boost/shared_ptr.hpp>

using namespace zhanmm;
using namespace std;

Mutex m_mutex;
std::atomic<int>  a(0);

void func() {
    MutexLocker l(m_mutex);
    ++a;
}



int main(void) {
    boost::shared_ptr<ScalingThreadPool> scalingThreadPool(ThreadPoolManager::newScalingThreadPool());
    sleep(1);
    std::cout << scalingThreadPool->GetThreadNum() << std::endl;
    // std::cout << a.load() << std::endl;
    // for(int index = 0; index <= 100000; ++index) {
    //     scalingThreadPool->AddTask(func);
    // }
    // std::cout << scalingThreadPool->GetThreadNum() << std::endl;

    // std::cout << a.load() << std::endl;

}