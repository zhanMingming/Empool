#include "System.h"

#include <cstdio>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>

namespace
{

    pid_t gettid()
    {
#if defined(__APPLE__)
        uint64_t tid64;
        pthread_threadid_np(NULL, &tid64);
        std::cout << "tid64:" << tid64 << std::endl;
        return (pid_t)tid64;
#else
        return static_cast<pid_t>(::syscall(SYS_gettid));
#endif
    }

}  // namespace

/*
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_12
uint64_t tid64;
pthread_threadid_np(NULL, &tid64);
pid_t tid = (pid_t)tid64;
#else
pid_t tid = syscall(__NR_gettid);
#endif
*/

namespace empool
{

    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char *t_threadName = "unknown";


    void CacheTid()
    {
        if (t_cachedTid == 0)
        {
            t_cachedTid = gettid();
            std::cout << "tid:" << t_cachedTid << std::endl;
            t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
        }
    }

    bool IsMainThread()
    {
        return Tid() == ::getpid();
    }


}  // namespace empool