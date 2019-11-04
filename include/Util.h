#ifndef  ZHANMM_UTIL_H
#define  ZHANMM_UTIL_H

#include <stdint.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

namespace  {

typedef  uint64_t  TimeValue;
typedef  int       ThreadId;

//#define LOG_MESSAGE(msg)  std::cout << 
void MilliSleep(TimeValue time_in_ms)
{
  struct timeval timeout;
  timeout.tv_sec = time_in_ms / 1000;
  timeout.tv_usec = (time_in_ms % 1000) * 1000;
  (void) select(0, NULL, NULL, NULL, &timeout);
}

void MicroSleep(TimeValue time_in_ms) {
  usleep(time_in_ms);
}

} //namespace zhanmm

#endif
