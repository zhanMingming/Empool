#include <empool/ThreadPoolManager.h>


using namespace empool;

void func1() {
    std::cout << "func1" << std::endl;
}

void func2() {
    std::cout << "func2" << std::endl;
}

void func3() {
    std::cout << "func3" << std::endl;
}

int main(int argc, char **argv) {

    std::cout << "---- FIFO run mode----" << std::endl;
    boost::shared_ptr<SingleThreadPool>  single1(ThreadPoolManager::newSingleThreadPool(FIFO));

    single1->AddTask(func1);
    single1->AddTask(func2);
    sleep(1);
    single1->ShutDownNow();


    std::cout << "---- LIFO run mode----" << std::endl;
    boost::shared_ptr<SingleThreadPool>  single2(ThreadPoolManager::newSingleThreadPool(LIFO));

    single2->AddTask(func1);
    single2->AddTask(func2);
    single2->AddTask(func3);
    sleep(1);
    single2->ShutDownNow();


    std::cout << "---- PRIORITY run mode----" << std::endl;
    boost::shared_ptr<SingleThreadPool>  single3(ThreadPoolManager::newSingleThreadPool(PRIORITY));

    single3->AddTask(func1, 100);
    single3->AddTask(func2, 10);
    sleep(1);
    single3->ShutDownNow();
 
}

