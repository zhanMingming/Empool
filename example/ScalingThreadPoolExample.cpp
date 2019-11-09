#include "ThreadPoolManager.h"
#include "Mutex.h"
#include "TaskBase.h"
#include "Util.h"

#include <atomic>
#include <iostream>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <boost/shared_ptr.hpp>

using namespace zhanmm;
using namespace std;

Mutex m_mutex;
std::atomic<int>  a(0);

void func()
{
    MutexLocker l(m_mutex);
    MilliSleep(1);
    ++a;
    //std::cout << a << std::endl;
}
// DEFINE_string(log_dir, "./log", "LOG");
// DEFINE_string(max_log_size, 10, "max log size");

int main(int argc, char *argv[])
{

    google::InitGoogleLogging(argv[0]);  // 初始化 glog
    google::ParseCommandLineFlags(&argc, &argv, true);  // 初始化 gflags
    FLAGS_log_dir = "./log";
    FLAGS_max_log_size = 10;
    //google::SetLogDestination(google::GLOG_INFO, FLAGS_log_dir);
    google::SetStderrLogging(google::GLOG_INFO);

    // LOG(INFO) << "Hello, GOOGLE!";  // INFO 级别的日志
    // LOG(ERROR) << "ERROR, GOOGLE!";  // ERROR 级别的日志

    boost::shared_ptr<ScalingThreadPool> scalingThreadPool(ThreadPoolManager::newScalingThreadPool());
    //sleep(1);
    std::cout << scalingThreadPool->GetThreadNum() << std::endl;
    std::cout << a.load() << std::endl;
    for(int index = 0; index <= 10000; ++index)
    {
        scalingThreadPool->AddTask(func);
    }

    std::cout << scalingThreadPool->GetThreadNum() << std::endl;
    //std::cout << scalingThreadPool->GetThreadNum() << std::endl;
    //std::cout << scalingThreadPool->GetThreadNum() << std::endl;

    std::cout << a.load() << std::endl;

    sleep(10);
    std::cout << a.load() << std::endl;
    std::cout << scalingThreadPool->GetThreadNum() << std::endl;


    sleep(35);
    std::cout << a.load() << std::endl;
    std::cout << scalingThreadPool->GetThreadNum() << std::endl;
    google::ShutdownGoogleLogging();
}