#include "Util.h"


namespace zhanmm
{
    void MilliSleep(TimeValue time_in_ms)
    {
        struct timeval timeout;
        timeout.tv_sec = time_in_ms / 1000;
        timeout.tv_usec = (time_in_ms % 1000) * 1000;
        (void) select(0, NULL, NULL, NULL, &timeout);
    }

    void MicroSleep(TimeValue time_in_ms)
    {
        usleep(time_in_ms);
    }

    long long timeInMilliseconds(void)
    {
        struct timeval tv;

        gettimeofday(&tv, NULL);
        return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
    }



    long timeInSeconds(void)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec;
        // return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);

    }

}