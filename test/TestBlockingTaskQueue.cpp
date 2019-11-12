#include "BlockingTaskQueue.h"
#include "Thread.h"
#include "TaskBase.h"
#include "Mutex.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace empool;


namespace
{
    struct EmptyTask : public TaskBase
    {
        void DoRun() {}
    };

    struct PushThreadFunc
    {
        const int num;
        BlockingTaskQueue &taskQueue;

        PushThreadFunc(BlockingTaskQueue &q, const int n)
            : num(n), taskQueue(q)
        {}

        virtual ~PushThreadFunc() {}

        virtual void DoRun()
        {
            for (int i = 0; i < num; ++i)
            {
                taskQueue.Push(boost::shared_ptr<TaskBase>(new EmptyTask));
            }
        }

        void operator()()
        {
            DoRun();
        }
    };
}

// TEST(BlockingTaskQueueTestSuite, test_Push)
// {
//     BlockingTaskQueue taskQueue;
//     {
//         Thread t1((PushThreadFunc(taskQueue, 20)));
//         //std::cout << "sd" << std::endl;
//         Thread t2((PushThreadFunc(taskQueue, 50)));
//     }

//     ASSERT_EQ(static_cast<size_t>(70), taskQueue.Size());
// }

namespace
{
    struct PopThreadFunc
    {
        BlockingTaskQueue &taskQueue;

        PopThreadFunc(BlockingTaskQueue &q)
            : taskQueue(q)
        {
        }

        void operator()()
        {
            boost::shared_ptr<TaskBase> b = taskQueue.Pop();
        }
    };
}

// TEST(BlockingTaskQueueTestSuite, test_PopWait)
// {
//     BlockingTaskQueue taskQueue;
//     {
//         Thread popThread((PopThreadFunc(taskQueue)));
//         sleep(1);
//         Thread pushThread((PushThreadFunc(taskQueue, 10)));
//     }

//     ASSERT_EQ(static_cast<size_t>(9), taskQueue.Size());
// }

namespace
{
    struct ThreadSafeInt
    {
        int i;
        Mutex mutex;

        ThreadSafeInt() : i(0) {}
        ThreadSafeInt &operator++()
        {
            MutexLocker l(mutex);
            ++i;
            return *this;
        }
    };

    struct IncTask : public TaskBase
    {
        ThreadSafeInt &counter;

        IncTask(ThreadSafeInt &i)
            : counter(i)
        {}

        void DoRun()
        {
            ++counter;
        }
    };

    struct PushIncTaskThreadFunc : public PushThreadFunc
    {
        ThreadSafeInt &counter;

        PushIncTaskThreadFunc(BlockingTaskQueue &q, int n, ThreadSafeInt &i)
            : PushThreadFunc(q, n), counter(i)
        {}

        virtual void DoRun()
        {
            for (int i = 0; i < num; ++i)
            {
                taskQueue.Push(boost::shared_ptr<TaskBase>(new IncTask(counter)));
            }
        }
    };

    struct RunTaskThreadFunc
    {
        BlockingTaskQueue &taskQueue;

        RunTaskThreadFunc(BlockingTaskQueue &q)
            : taskQueue(q)
        {}

        void operator()()
        {
            boost::shared_ptr<TaskBase> t = taskQueue.Pop();
            t->Run();
        }
    };
}

// TEST(BlockingTaskQueueTestSuite, test_NormalUsage)
// {
//     BlockingTaskQueue taskQueue;
//     ThreadSafeInt counter;
//     {
//         typedef boost::shared_ptr<Thread> ThreadPtr;
//         vector<ThreadPtr> workers;

//         for (int i = 0; i < 10; ++i)
//         {
//             workers.push_back(ThreadPtr(
//                                   new Thread((RunTaskThreadFunc(taskQueue)))));
//         }
//         Thread t2((PushIncTaskThreadFunc(taskQueue, 10, counter)));
//     }

//     ASSERT_EQ(10, counter.i);
// }


// GTEST_API_ int main(int argc, char **argv)
// {
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }


