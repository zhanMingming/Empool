#include "ThreadPoolManager.h"
#include <iostream>

using namespace empool;
using namespace std;

void func() {
    std::cout << "func" << std::endl;
}


int main(void) {


    boost::shared_ptr<ScheduledThreadPool> schedule1(ThreadPoolManager::newScheduledThreadPool(2));

    schedule1->AddCronTimerTask(func, 100);

    sleep(1);

    schedule1->ShutDownNow();


    {
        boost::shared_ptr<ScheduledThreadPool> schedule2(ThreadPoolManager::newScheduledThreadPool(4));

        schedule2->AddCycleTimerTask(func, 10);
        sleep(1);

        schedule2->ShutDown();

    }

    boost::shared_ptr<ScheduledThreadPool> schedule3(ThreadPoolManager::newScheduledThreadPool(8));

    schedule3->AddCycleTimerTask(func, 10);
    sleep(1);

    schedule3->ShutDown();

}