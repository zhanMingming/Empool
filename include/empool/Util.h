#ifndef  ZHANMM_UTIL_H
#define  ZHANMM_UTIL_H

#include <stdint.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>


namespace empool
{

    typedef  uint64_t  TimeValue;
    typedef  int       ThreadId;

    //#define LOG_MESSAGE(msg)  std::cout <<
    void MilliSleep(TimeValue time_in_ms);

    void MicroSleep(TimeValue time_in_ms);


    long long timeInMilliseconds(void);


    long timeInSeconds(void);

} //namespace empool

#endif
