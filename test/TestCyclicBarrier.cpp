#include <iostream>
#include <thread>
#include"CyclicBarrier.h"

using namespace empool;

CyclicBarrier barrier(2);

void doWork()
{
    barrier.Await();
    std::cout << "thread1 common" << std::endl;
}

// int main()
// {
//     // CyclicBarrier barrier(2);
//     std::thread th1(doWork);
//     std::thread th2(doWork);

//     th1.join();
//     th2.join();

// }