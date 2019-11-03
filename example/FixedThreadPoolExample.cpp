/*
* @Author: zhanmingming
* @Date:   2019-11-03 11:01:18
* @Last Modified by:   zhanmingming
* @Last Modified time: 2019-11-03 11:10:50
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
    boost::shared_ptr<LFixedThreadPool> lFixedThreadPool(ThreadPoolManager::newFixedThreadPool());
    sleep(1);
    std::cout << lFixedThreadPool->GetThreadNum() << std::endl;
    // std::cout << a.load() << std::endl;
    // for(int index = 0; index <= 100000; ++index) {
    //     LFixedThreadPool->AddTask(func);
    // }
    // std::cout << LFixedThreadPool->GetThreadNum() << std::endl;

    // std::cout << a.load() << std::endl;

}