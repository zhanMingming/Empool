#include "TimerTaskHandler.h"
#include "CloseableThread.h"
//#include "TestUtil.h"

#include <sys/select.h>
#include <atomic>
#include <gtest/gtest.h>

using namespace zhanmm;
using namespace std;



namespace
{
    class TimerTaskHandlerTestSuite : public testing::Test
    {
    protected:
        atomic<int> counter;

        TimerTaskHandlerTestSuite()
            : counter(0)
        {}

        virtual void SetUp()
        {
            counter = 0;
        }
    };

    struct TestTimerTaskHandlerTask : public TimerTask
    {
        atomic<int> &counter;

        TestTimerTaskHandlerTask(atomic<int> &cnt)
            : counter(cnt)
        {}

        virtual void DoRun()
        {
            ++counter;
        }
    };
}

TEST_F(TimerTaskHandlerTestSuite, test_Ctor)
{
    TimerTaskHandler timerTaskHandler;
}


TEST_F(TimerTaskHandlerTestSuite, test_AddCronTimerTask)
{
    {
        TimerTaskHandler timerTaskHandler;
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 200);
        ASSERT_EQ(0, counter);
        MilliSleep(300);
        //timerTaskHandler.ShutDown( );
        ASSERT_EQ(1, counter);
    }
    ASSERT_EQ(1, counter);
}


TEST_F(TimerTaskHandlerTestSuite, test_AddCronTimerTask_and_cancel_task)
{
    {
        TimerTaskHandler timerTaskHandler;
        boost::shared_ptr<TimerTask> task(new TestTimerTaskHandlerTask(counter));
        timerTaskHandler.AddCronTimerTask(task, 200);
        ASSERT_EQ(0, counter);

        task->Cancel();
        ASSERT_EQ(0, counter);
    }
    ASSERT_EQ(0, counter);
}

TEST_F(TimerTaskHandlerTestSuite, test_AddCronTimerTask_and_async_cancel_task)
{
    {
        TimerTaskHandler timerTaskHandler;
        boost::shared_ptr<TimerTask> task(new TestTimerTaskHandlerTask(counter));
        timerTaskHandler.AddCronTimerTask(task, 200);
        ASSERT_EQ(0, counter);

        task->Cancel();
        ASSERT_EQ(0, counter);
    }
    ASSERT_EQ(0, counter);
}

TEST_F(TimerTaskHandlerTestSuite, test_AddCronTimerTask_with_same_task)
{
    {
        TimerTaskHandler timerTaskHandler;
        boost::shared_ptr<TimerTask> task(new TestTimerTaskHandlerTask(counter));
        ASSERT_TRUE(timerTaskHandler.AddCronTimerTask(task, 200));
        ASSERT_FALSE(timerTaskHandler.AddCronTimerTask(task, 200));
        ASSERT_EQ(0, counter);

        MilliSleep(300);
        ASSERT_EQ(1, counter);
    }
    ASSERT_EQ(1, counter);
}

TEST_F(TimerTaskHandlerTestSuite, test_AddCycleTimerTask)
{
    {
        TimerTaskHandler timerTaskHandler;
        boost::shared_ptr<TimerTask> task(new TestTimerTaskHandlerTask(counter));
        timerTaskHandler.AddCycleTimerTask(task, 500, true);
        MilliSleep(100);
        ASSERT_EQ(1, counter);

        MilliSleep(1100);
        ASSERT_EQ(3, counter);
    }
    ASSERT_EQ(3, counter);
}

// TEST_F(TimerTaskHandlerTestSuite, test_AddCycleTimerTask_with_same_task)
// {
//   {
//     TimerTaskHandler timerTaskHandler;
//     boost::shared_ptr<TimerTask> task(new TestTimerTaskHandlerTask(counter));
//     ASSERT_TRUE(timerTaskHandler.AddCycleTimerTask(task, 500, true));
//     ASSERT_FALSE(timerTaskHandler.AddCycleTimerTask(task, 500, true));
//     MilliSleep(100);
//     ASSERT_EQ(1, counter);

//     MilliSleep(1100);
//     ASSERT_EQ(3, counter);
//   }
//   ASSERT_EQ(3, counter);
// }

TEST_F(TimerTaskHandlerTestSuite, test_AddCycleTimerTask_and_not_run_now)
{
    {
        TimerTaskHandler timerTaskHandler;
        boost::shared_ptr<TimerTask> task(new TestTimerTaskHandlerTask(counter));
        timerTaskHandler.AddCycleTimerTask(task, 500, false);
        MilliSleep(100);
        ASSERT_EQ(0, counter);

        MilliSleep(1100);
        ASSERT_EQ(2, counter);
    }
    ASSERT_EQ(2, counter);
}

TEST_F(TimerTaskHandlerTestSuite, test_AddCycleTimerTask_and_cancel)
{
    {
        TimerTaskHandler timerTaskHandler;
        boost::shared_ptr<TimerTask> task(new TestTimerTaskHandlerTask(counter));
        timerTaskHandler.AddCycleTimerTask(task, 500, true);
        MilliSleep(100);
        ASSERT_EQ(1, counter);

        MilliSleep(500);
        ASSERT_EQ(2, counter);
        task->Cancel();

        MilliSleep(500);
        ASSERT_EQ(2, counter);
    }
    ASSERT_EQ(2, counter);
}

TEST_F(TimerTaskHandlerTestSuite, test_Run_with_multiple_tasks)
{
    {
        TimerTaskHandler timerTaskHandler;
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 200);
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 100);
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 300);
        ASSERT_EQ(0, counter);

        MilliSleep(200);
        ASSERT_GT(counter, 0);

        MilliSleep(500);
        ASSERT_EQ(3, counter);
    }
    ASSERT_EQ(3, counter);
}

namespace
{
    struct TestTimerTaskHandlerFunc
    {
        atomic<int> &counter;

        TestTimerTaskHandlerFunc(atomic<int> &cnt)
            : counter(cnt)
        {}

        void operator()()
        {
            ++counter;
        }
    };
}

TEST_F(TimerTaskHandlerTestSuite, test_AddCronTimerTask_with_functor)
{
    {
        TimerTaskHandler timerTaskHandler;
        timerTaskHandler.AddCronTimerTask(TestTimerTaskHandlerFunc(counter), 200);
        ASSERT_EQ(0, counter);

        MilliSleep(500);
        ASSERT_EQ(1, counter);
    }
    ASSERT_EQ(1, counter);
}

namespace
{
    struct AddCronTimerTaskThread
    {
        TimerTaskHandler &m_TimerTaskHandler;
        atomic<int> &m_counter;
        TimeValue m_delay;

        AddCronTimerTaskThread(TimerTaskHandler &TimerTaskHandler, atomic<int> &counter,
                               TimeValue delay)
            : m_TimerTaskHandler(TimerTaskHandler), m_counter(counter), m_delay(delay)
        {}

        void operator()()
        {
            m_TimerTaskHandler.AddCronTimerTask(TestTimerTaskHandlerFunc(m_counter), m_delay);
        }
    };
}

TEST_F(TimerTaskHandlerTestSuite, test_AddCronTimerTask_with_multiple_threads)
{
    {
        TimerTaskHandler timerTaskHandler;

        // create 2 threads to do AddCronTimerTask
        Thread thread1(AddCronTimerTaskThread(timerTaskHandler, counter, 100));
        Thread thread2(AddCronTimerTaskThread(timerTaskHandler, counter, 100));
        timerTaskHandler.AddCronTimerTask(TestTimerTaskHandlerFunc(counter), 100);

        MilliSleep(300);
        ASSERT_EQ(3, counter);
    }
    ASSERT_EQ(3, counter);
}

TEST_F(TimerTaskHandlerTestSuite, test_AddCycleTimerTask_with_functor)
{
    {
        TimerTaskHandler timerTaskHandler;
        timerTaskHandler.AddCycleTimerTask(TestTimerTaskHandlerFunc(counter), 500, false);
        ASSERT_EQ(0, counter);

        MilliSleep(600);
        ASSERT_EQ(1, counter);

        MilliSleep(500);
        ASSERT_EQ(2, counter);
    }
    ASSERT_EQ(2, counter);
}

namespace
{
    struct AddCycleTimerTaskThread
    {
        TimerTaskHandler &m_TimerTaskHandler;
        atomic<int> &m_counter;
        TimeValue m_delay;
        bool m_is_run_now;

        AddCycleTimerTaskThread(TimerTaskHandler &TimerTaskHandler, atomic<int> &counter,
                                TimeValue delay, bool is_run_now)
            : m_TimerTaskHandler(TimerTaskHandler), m_counter(counter), m_delay(delay),
              m_is_run_now(is_run_now)
        {}

        void operator()()
        {
            m_TimerTaskHandler.AddCycleTimerTask(TestTimerTaskHandlerFunc(m_counter), m_delay,
                                                 m_is_run_now);
        }
    };
}

TEST_F(TimerTaskHandlerTestSuite, test_AddCycleTimerTask_with_multiple_threads)
{
    {
        TimerTaskHandler timerTaskHandler;

        // create 2 threads to do AddCycleTimerTask
        Thread thread1(AddCycleTimerTaskThread(timerTaskHandler, counter, 200, true));
        Thread thread2(AddCycleTimerTaskThread(timerTaskHandler, counter, 200, true));
        timerTaskHandler.AddCycleTimerTask(TestTimerTaskHandlerFunc(counter), 200, true);

        MilliSleep(100);
        ASSERT_EQ(3, counter);

        MilliSleep(200);
        ASSERT_EQ(6, counter);
    }
    ASSERT_EQ(6, counter);
}

// namespace {
//   struct SelfCancelTimerTaskHandlerTask : public TimerTaskHandlerTask
//   {
//     atomic<int>& counter;
//     const int cancel_count;

//     SelfCancelTimerTaskHandlerTask(atomic<int>& cnt, int cancel_cnt)
//     : counter(cnt), cancel_count(cancel_cnt)
//     {}

//     virtual void DoRun()
//     {
//       ++counter;
//       if (counter == cancel_count)
//       {
//         Cancel();
//       }
//     }
//   };
// }

// TEST_F(TimerTaskHandlerTestSuite, test_AddCycleTimerTask_with_self_cancel_task)
// {
//   {
//     TimerTaskHandler timerTaskHandler;
//     boost::shared_ptr<TimerTask> task(new SelfCancelTimerTaskHandlerTask(counter, 3));
//     timerTaskHandler.AddCycleTimerTask(task, 200, false);
//     MilliSleep(100);
//     ASSERT_EQ(0, counter);

//     while (!(task->IsCancelled()))
//     {
//       MilliSleep(200);
//     }
//     ASSERT_EQ(3, counter);
//   }
//   ASSERT_EQ(3, counter);
// }

TEST_F(TimerTaskHandlerTestSuite, test_StopAsync)
{
    {
        TimerTaskHandler timerTaskHandler;
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 500);
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 100);
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 1000);
        ASSERT_EQ(0, counter);

        MilliSleep(200);
        ASSERT_EQ(1, counter);

        timerTaskHandler.ShutDown( );
    }
    ASSERT_EQ(1, counter);
}

TEST_F(TimerTaskHandlerTestSuite, test_AddCronTimerTask_after_StopAsync)
{
    {
        TimerTaskHandler timerTaskHandler;
        boost::shared_ptr<TimerTask> task(new TestTimerTaskHandlerTask(counter));
        ASSERT_TRUE(timerTaskHandler.AddCronTimerTask(task, 100));
        ASSERT_EQ(0, counter);

        MilliSleep(300);
        ASSERT_EQ(1, counter);

        timerTaskHandler.ShutDown( );
        boost::shared_ptr<TimerTask> task1(new TestTimerTaskHandlerTask(counter));
        ASSERT_FALSE(timerTaskHandler.AddCronTimerTask(task1, 100));
        ASSERT_EQ(1, counter);

        MilliSleep(300);
        ASSERT_EQ(1, counter);
    }
    ASSERT_EQ(1, counter);
}

TEST_F(TimerTaskHandlerTestSuite, test_Stop)
{
    {
        TimerTaskHandler timerTaskHandler;
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 500);
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 100);
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 1000);
        ASSERT_EQ(0, counter);

        MilliSleep(200);
        ASSERT_EQ(1, counter);

        timerTaskHandler.ShutDown( );

        MilliSleep(500);
        ASSERT_EQ(1, counter);
    }
    ASSERT_EQ(1, counter);
}

namespace
{
    struct TimerTaskHandlerStopThread
    {
        TimerTaskHandler &m_TimerTaskHandler;

        TimerTaskHandlerStopThread(TimerTaskHandler &TimerTaskHandler)
            : m_TimerTaskHandler(TimerTaskHandler)
        {}

        void operator()()
        {
            m_TimerTaskHandler.ShutDown( );
        }
    };
}

TEST_F(TimerTaskHandlerTestSuite, test_Stop_with_multiple_threads)
{
    {
        TimerTaskHandler timerTaskHandler;
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 500);
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 100);
        timerTaskHandler.AddCronTimerTask(boost::shared_ptr<TimerTask>(new TestTimerTaskHandlerTask(counter)), 1000);
        ASSERT_EQ(0, counter);

        MilliSleep(200);
        ASSERT_EQ(1, counter);

        Thread thread1((TimerTaskHandlerStopThread(timerTaskHandler)));
        Thread thread2((TimerTaskHandlerStopThread(timerTaskHandler)));
        timerTaskHandler.ShutDown( );

        MilliSleep(500);
        ASSERT_EQ(1, counter);
    }
    ASSERT_EQ(1, counter);
}

TEST_F(TimerTaskHandlerTestSuite, test_AddCronTimerTask_after_Stop)
{
    {
        TimerTaskHandler timerTaskHandler;
        boost::shared_ptr<TimerTask> task(new TestTimerTaskHandlerTask(counter));
        ASSERT_TRUE(timerTaskHandler.AddCronTimerTask(task, 100));
        ASSERT_EQ(0, counter);

        MilliSleep(300);
        ASSERT_EQ(1, counter);

        timerTaskHandler.ShutDown( );
        boost::shared_ptr<TimerTask> task1(new TestTimerTaskHandlerTask(counter));
        ASSERT_FALSE(timerTaskHandler.AddCronTimerTask(task1, 100));
        ASSERT_EQ(1, counter);

        MilliSleep(300);
        ASSERT_EQ(1, counter);
    }
    ASSERT_EQ(1, counter);
}


// GTEST_API_ int main(int argc, char **argv)
// {
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }


