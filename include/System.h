#ifndef ZHANMM_SYSTEM_H_
#define ZHANMM_SYSTEM_H_


namespace zhanmm {

#if defined(__GNUC__)
#define TPOOL_LIKELY(x)   (__builtin_expect((x), 1))
#define TPOOL_UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define TPOOL_LIKELY(x) (x)
#define TPOOL_UNLIKELY(x) (x)
#endif


extern __thread int t_cachedTid;
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;
extern __thread const char* t_threadName;
void CacheTid();

inline int Tid()
{
    if (TPOOL_UNLIKELY(t_cachedTid == 0))
    {
        CacheTid();
    }
    return t_cachedTid;
}

inline const char* TidString()
{
    return t_tidString;
}

inline int TidStringLength()
{
    return t_tidStringLength;
}

bool IsMainThread();


}  // namespace zhanmm


#endif