#include "NoBlockingTaskQueue.h"
#include "Thread.h"
#include "TaskBase.h"
#include "Mutex.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <vector>

using namespace std;
using namespace zhanmm;


namespace
{
    struct EmptyTask : public TaskBase
    {
        void DoRun() {}
    };

    struct PushThreadFunc
    {
        const int num;
        NoBlockingTaskQueue &noBlockingTaskQueue;

        PushThreadFunc(NoBlockingTaskQueue &q, const int n)
            : num(n), noBlockingTaskQueue(q)
        {}

        virtual ~PushThreadFunc() {}

        virtual void DoRun()
        {
            for (int i = 0; i < num; ++i)
            {
                noBlockingTaskQueue.Push(new EmptyTask);
            }
        }

        void operator()()
        {
            DoRun();
        }
    };
}

// TEST(NoBlockingTaskQueueTestSuite, test_Push)
// {
//   NoBlockingTaskQueue noBlockingTaskQueue;
//   {
//     Thread t1((PushThreadFunc(noBlockingTaskQueue, 20)));
//     Thread t2((PushThreadFunc(noBlockingTaskQueue, 50)));
//   }

//   ASSERT_EQ(static_cast<size_t>(70), noBlockingTaskQueue.Size());
// }


namespace
{
    struct PopThreadFunc
    {
        NoBlockingTaskQueue &noBlockingTaskQueue;

        PopThreadFunc(NoBlockingTaskQueue &q)
            : noBlockingTaskQueue(q)
        {
        }

        void operator()()
        {
            boost::shared_ptr<TaskBase> b = noBlockingTaskQueue.Pop();
        }
    };
}

// TEST(NoBlockingTaskQueueTestSuite, test_PopWait)
// {
//   NoBlockingTaskQueue noBlockingTaskQueue;
//   {
//     Thread popThread((PopThreadFunc(noBlockingTaskQueue)));
//     sleep(1);
//     Thread pushThread((PushThreadFunc(noBlockingTaskQueue, 10)));
//   }

//   ASSERT_EQ(static_cast<size_t>(9), noBlockingTaskQueue.Size());
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

        PushIncTaskThreadFunc(NoBlockingTaskQueue &q, int n, ThreadSafeInt &i)
            : PushThreadFunc(q, n), counter(i)
        {}

        virtual void DoRun()
        {
            for (int i = 0; i < num; ++i)
            {
                noBlockingTaskQueue.Push(new IncTask(counter));
            }
        }
    };

    struct RunTaskThreadFunc
    {
        NoBlockingTaskQueue &noBlockingTaskQueue;

        RunTaskThreadFunc(NoBlockingTaskQueue &q)
            : noBlockingTaskQueue(q)
        {}

        void operator()()
        {
            boost::shared_ptr<TaskBase> t = noBlockingTaskQueue.Pop();
            //std::cout << "error e" << std::endl;
            t->Run();
        }
    };
}

TEST(NoBlockingTaskQueueTestSuite, test_NormalUsage)
{
    NoBlockingTaskQueue noBlockingTaskQueue;
    ThreadSafeInt counter;
    {
        typedef boost::shared_ptr<Thread> ThreadPtr;
        vector<ThreadPtr> workers;

        for (int i = 0; i < 10; ++i)
        {
            workers.push_back(ThreadPtr(
                                  new Thread((RunTaskThreadFunc(noBlockingTaskQueue)))));
        }
        Thread t2((PushIncTaskThreadFunc(noBlockingTaskQueue, 10, counter)));
    }

    ASSERT_EQ(10, counter.i);
}


// GTEST_API_ int main(int argc, char **argv)
// {
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }


