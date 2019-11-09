/*
* @Author: zhanmingming
* @Date:   2019-11-09 23:43:17
* @Last Modified by:   zhanmingming
* @Last Modified time: 2019-11-09 23:52:30
*/
#include "ThreadPoolManager.h"
#include "Util.h"


using namespace zhanmm;

void func1() {
    std::cout << "func1" << std::endl;
}

void func2() {
    std::cout << "func2" << std::endl;
}


int main(int argc, char **argv) {

    boost::shared_ptr<SingleThreadPool>  singleThreadPool(ThreadPoolManager::newSingleThreadPool(FIFO));

    singleThreadPool->AddTask(func1);
    singleThreadPool->AddTask(func2);
    
 
}

