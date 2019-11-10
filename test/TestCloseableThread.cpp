#include "CloseableThread.h"

#include <boost/function.hpp>

#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

using namespace std;

typedef ::boost::function<void()> CloseFunction;

namespace
{

    void NoOpWork(CloseFunction)
    {}

    void NoOp(int threadId)
    {}

    class IncNumWork
    {
        int &m_num;
    public:
        IncNumWork(int &num)
            : m_num(num)
        {}

        void operator() (::zhanmm::CloseableThread::Function)
        {
            ++m_num;
        }
    };

}

TEST(CloseableThread, test_Ctor)
{
    {
        ::zhanmm::CloseableThread thread((&NoOpWork));
    }

    {
        ::zhanmm::CloseableThread thread(&NoOpWork, &NoOp);
    }
}

TEST(CloseableThread, test_normal_usage)
{
    int num = 0;
    {
        ::zhanmm::CloseableThread thread((IncNumWork(num)));
    }

    ASSERT_EQ(1, num);
}

namespace
{

    struct TestWork
    {
        int &counter;

        TestWork(int &i)
            : counter(i)
        {}

        void operator() (::zhanmm::CloseableThread::Function CloseFunction)
        {
            sleep(2);
            CloseFunction();
            ++counter;
        }
    };

}


TEST(CloseableThread, test_Close)
{
    int counter = 0;
    {
        ::zhanmm::CloseableThread t((TestWork(counter)));
        sleep(1);
        t.Close();
        ASSERT_EQ(0, counter);
    }
    ASSERT_EQ(0, counter);
}

TEST(CloseableThread, test_multiple_Close)
{
    int counter = 0;
    {
        ::zhanmm::CloseableThread t((TestWork(counter)));
        sleep(1);
        t.Close();
        t.Close(); // should not block the execution.
    }
    ASSERT_EQ(0, counter);
}


namespace
{

    struct FinishAction
    {
        int &counter;

        FinishAction(int &i)
            : counter(i)
        {}

        void operator()(int threadId)
        {
            ++counter;
        }
    };

}

TEST(CloseableThread, test_ctor_with_FinishAction)
{
    int counter = 0;
    int finishCounter = 0;
    {
        ::zhanmm::CloseableThread t((TestWork(counter)),
                                    FinishAction(finishCounter));
    }
    ASSERT_EQ(1, counter);
    ASSERT_EQ(1, finishCounter);
}

TEST(CloseableThread, test_FinishAcion_execute_before_Finished)
{
    int counter = 0;
    int finishFlag = 0;
    {
        ::zhanmm::CloseableThread t((TestWork(counter)),
                                    FinishAction(finishFlag));
        sleep(1);
        t.Close();
        EXPECT_EQ(0, counter);
        EXPECT_EQ(1, finishFlag);
    }
    EXPECT_EQ(0, counter);
    EXPECT_EQ(1, finishFlag);
}


// GTEST_API_ int main(int argc, char **argv)
// {
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }

