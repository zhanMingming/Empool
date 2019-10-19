#include "Thread.h"

#include <gtest/gtest.h>

#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <cstdlib>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace zhanmm;

namespace {
int GLOBAL_COUNTER = 0;

struct ThreadFunctor {
    void operator()()
    {
        for (int i = 0; i < 20; ++i)
        {
            ++GLOBAL_COUNTER;
        }
    }
};

}


TEST(BasicThreadTestSuite, test_lifecycle)
{
    {
        Thread t((ThreadFunctor()));
    }

    ASSERT_EQ(20, GLOBAL_COUNTER);
}

namespace {
struct ThreadFuncThrowException {
    explicit ThreadFuncThrowException(int& num)
    : m_num(num)
    {
    }

    virtual ~ThreadFuncThrowException() {}

    void operator()()
    {
        m_num = 0;
        ThrowException();
        m_num = 1;
    }

    virtual void ThrowException()
    {
        throw runtime_error("exit thread function");
    }

    int& m_num;
};
}

TEST(BasicThreadTestSuite, test_threadFuncThrowException)
{
    int i = -1;
    ThreadFuncThrowException f(i);

    {
        Thread t(f);
    }

    ASSERT_EQ(0, i);
}



GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


