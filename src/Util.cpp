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

}