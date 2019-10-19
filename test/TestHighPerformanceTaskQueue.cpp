#include "HighPerformanceTaskQueue.h"
#include "Thread.h"
#include "TaskBase.h"
#include "Mutex.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <vector>

using namespace std;
using namespace zhanmm;


namespace {
  struct EmptyTask : public TaskBase {
    void DoRun() {}
  };

  struct PushThreadFunc {
    const int num;
    HighPerformanceTaskQueue& highPerformanceTaskQueue;
    
    PushThreadFunc(HighPerformanceTaskQueue& q, const int n)
      : num(n), highPerformanceTaskQueue(q)
    {}

    virtual ~PushThreadFunc() {}

    virtual void DoRun()
    {
      for (int i = 0; i < num; ++i)
    {
      highPerformanceTaskQueue.Push(new EmptyTask);
    }
    }
    
    void operator()()
    {
      DoRun();
    }
  };
}

// TEST(HighPerformanceTaskQueueTestSuite, test_Push)
// {
//   HighPerformanceTaskQueue highPerformanceTaskQueue;
//   {
//     Thread t1((PushThreadFunc(highPerformanceTaskQueue, 20)));
//     Thread t2((PushThreadFunc(highPerformanceTaskQueue, 50)));
//   }

//   ASSERT_EQ(static_cast<size_t>(70), highPerformanceTaskQueue.Size());
// }


namespace {
  struct PopThreadFunc {
    HighPerformanceTaskQueue& highPerformanceTaskQueue;
    
    PopThreadFunc(HighPerformanceTaskQueue& q)
      : highPerformanceTaskQueue(q)
    {
    }

    void operator()()
    {
      boost::shared_ptr<TaskBase> b = highPerformanceTaskQueue.Pop();
    }
  };
}

// TEST(HighPerformanceTaskQueueTestSuite, test_PopWait)
// {
//   HighPerformanceTaskQueue highPerformanceTaskQueue;
//   {
//     Thread popThread((PopThreadFunc(highPerformanceTaskQueue)));
//     sleep(1);
//     Thread pushThread((PushThreadFunc(highPerformanceTaskQueue, 10)));
//   }

//   ASSERT_EQ(static_cast<size_t>(9), highPerformanceTaskQueue.Size());
// }

namespace {
  struct ThreadSafeInt {
    int i;
    Mutex mutex;

    ThreadSafeInt() : i(0) {}
    ThreadSafeInt& operator++()
    {
      MutexLocker l(mutex);
      ++i;
      return *this;
    }
  };
  
  struct IncTask : public TaskBase {
    ThreadSafeInt& counter;

    IncTask(ThreadSafeInt& i)
      : counter(i)
    {}

    void DoRun()
    {
      ++counter;
    }
  };

  struct PushIncTaskThreadFunc : public PushThreadFunc {
    ThreadSafeInt& counter;

    PushIncTaskThreadFunc(HighPerformanceTaskQueue& q, int n, ThreadSafeInt& i)
      : PushThreadFunc(q, n), counter(i)
    {}

    virtual void DoRun()
    {
      for (int i = 0; i < num; ++i)
    {
      highPerformanceTaskQueue.Push(new IncTask(counter));
    }
    }
  };

  struct RunTaskThreadFunc {
    HighPerformanceTaskQueue& highPerformanceTaskQueue;

    RunTaskThreadFunc(HighPerformanceTaskQueue& q)
      : highPerformanceTaskQueue(q)
    {}

    void operator()()
    {
      boost::shared_ptr<TaskBase> t = highPerformanceTaskQueue.Pop();
      //std::cout << "error e" << std::endl;
      t->Run();
    }
  };
}

TEST(HighPerformanceTaskQueueTestSuite, test_NormalUsage)
{
  HighPerformanceTaskQueue highPerformanceTaskQueue;
  ThreadSafeInt counter;
  {
    typedef boost::shared_ptr<Thread> ThreadPtr;
    vector<ThreadPtr> workers;

    for (int i = 0; i < 10; ++i)
      {
    workers.push_back(ThreadPtr(
      new Thread((RunTaskThreadFunc(highPerformanceTaskQueue)))));
      }
    Thread t2((PushIncTaskThreadFunc(highPerformanceTaskQueue, 10, counter)));
  }

  ASSERT_EQ(10, counter.i);
}


GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


