#ifndef ZHANMM_CYCLICBARRIER_H_
#define ZHANMM_CYCLICBARRIER_H_

#include "ConditionVariable.h"

#include <boost/noncopyable.hpp>

#include <cassert>
#include <atomic>

namespace zhanmm {

enum  State
{
    BROKEN  =  -2,
    TIMEOUT = -1,
    SUCCESS  = 0
};



class CyclicBarrier : private boost::noncopyable {
public:
    CyclicBarrier(const int count);
    ~CyclicBarrier() {}


    int Await();
    // millseconds
    int Await(int time_ms);

    void Reset();

private:
    const int m_max_count;
    int  m_count;
    int  m_reset_index;
    int  m_barrier_cond;
    ConditionVariable m_condition_variable;
    Mutex m_mutex;
};

}  //zhanmm


#endif /* ZHANMM_CYCLICBARRIER_H_ */
