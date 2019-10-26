#include "FixedThreadPool.h"

#include <gtest/gtest.h>
#include <boost/bind.hpp>
#include <unistd.h>

using namespace zhanmm;
using namespace boost;
using namespace std;

void MilliSleep(TimeValue time_in_ms)
{
  struct timeval timeout;
  timeout.tv_sec = time_in_ms / 1000;
  timeout.tv_usec = (time_in_ms % 1000) * 1000;
  (void) select(0, NULL, NULL, NULL, &timeout);
}



TEST(FixedThreadPool, test_Construction)
{
  {
    LFixedThreadPool threadPool;
    EXPECT_EQ(static_cast<size_t>(8), threadPool.GetThreadNum());
  }
  
  {
    LFixedThreadPool threadPool(5);
    EXPECT_EQ(static_cast<size_t>(5), threadPool.GetThreadNum());
  }
}

namespace {
  Mutex GLOBAL_MUTEX;
  
  struct IncTask : public TaskBase {
    int& counter;
    
    IncTask(int& i) : counter(i) {}

    virtual void DoRun()
    {
      MutexLocker l(GLOBAL_MUTEX);
      ++counter;
    }
  };
}

TEST(FixedThreadPool, test_AddTask)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    threadPool.AddTask(boost::shared_ptr<TaskBase>(new IncTask(counter)));
    sleep(1);
  }
  ASSERT_EQ(1, counter);
}

namespace {
  void IncrementWith(int* i)
  {
    //std::cout << "ff" << std::endl;
    *i = 1;
  }
}

TEST(FixedThreadPool, test_template_AddTask)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    threadPool.AddTask(bind(IncrementWith, &counter));
    sleep(1);
  }
  ASSERT_EQ(1, counter);
}

namespace {
  struct SleepFunc {
    void operator()()
    {
      sleep(1);
    }
  };

  struct SleepAndIncTask : public IncTask {
    SleepAndIncTask(int& i)
      : IncTask(i)
    {}
    
    virtual void DoRun()
    {
      sleep(1);
      IncTask::DoRun();
    }
  };
}

TEST(FixedThreadPool, test_AsyncStop)
{
  {
    LFixedThreadPool threadPool;
    threadPool.AddTask(SleepFunc());
    threadPool.AsyncStop();
    EXPECT_FALSE(threadPool.AddTask(SleepFunc()));
  }

  int counter = 0;
  {
    LFixedThreadPool threadPool;
    threadPool.AddTask(boost::shared_ptr<TaskBase>(new SleepAndIncTask(counter)));
    threadPool.AsyncStop();
    EXPECT_EQ(0, counter);
    sleep(3);
  }
  EXPECT_EQ(0, counter);
}

TEST(FixedThreadPool, test_Stop)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    threadPool.AddTask(boost::shared_ptr<TaskBase>(new SleepAndIncTask(counter)));
    EXPECT_EQ(0, counter);
    sleep(1);
    threadPool.Stop();
    EXPECT_EQ(1, counter);
    EXPECT_FALSE(threadPool.AddTask(boost::shared_ptr<TaskBase>(new IncTask(counter))));
  }
}

TEST(FixedThreadPool, test_Stop_when_TaskQueue_empty)
{
  {
    LFixedThreadPool threadPool;
    threadPool.Stop();
  }
}

namespace {
  struct StopFunction {
    LFixedThreadPool& m_threadPool;

    StopFunction(LFixedThreadPool& threadPool)
      : m_threadPool(threadPool)
    {}
    
    void operator()()
    {
      m_threadPool.Stop();
    }
  };
}

TEST(FixedThreadPool, test_multiple_Stop_simultinuously)
{
  {
    LFixedThreadPool threadPool;
    Thread t((StopFunction(threadPool)));
    threadPool.Stop();
  }
}

namespace {
  struct LoopSleepAndIncTask : public SleepAndIncTask {
    LoopSleepAndIncTask(int& i)
      : SleepAndIncTask(i)
    {}
    
    virtual void DoRun()
    {
      for (int i = 0; i < 2; ++i)
    {
      SleepAndIncTask::DoRun();
      sleep(1);
      CheckCancellation();
    }
    }
  };
}

TEST(FixedThreadPool, test_StopNow)
{
  int counter = 0;
  {
    LFixedThreadPool threadPool;
    const size_t num = threadPool.GetThreadNum();
    for (size_t i = 0; i < num; ++i)
      {
    threadPool.AddTask(boost::shared_ptr<TaskBase>(new LoopSleepAndIncTask(counter)));
      }
    sleep(1);
    threadPool.Stop();

    EXPECT_GE(static_cast<size_t>(counter), num);
    EXPECT_FALSE(threadPool.AddTask(boost::shared_ptr<TaskBase>(new IncTask(counter))));
  }
}

TEST(FixedThreadPool, test_StopNow_when_TaskQueue_empty)
{
  {
    LFixedThreadPool threadPool;
    threadPool.Stop();
  }
}

// TEST(FixedThreadPool, test_AddTimerTask)
// {
//   int counter = 0;
//   {
//     LFixedThreadPool threadPool;
//     threadPool.AddTimerTask(boost::shared_ptr<TaskBase>(new IncTask(counter)), 300);
//     ASSERT_EQ(0, counter);

//     MilliSleep(100);
//     ASSERT_EQ(0, counter);

//     MilliSleep(300);
//     ASSERT_EQ(1, counter);
//   }
//   ASSERT_EQ(1, counter);
// }

// TEST(FixedThreadPool, test_AddTimerTask_and_CancelAsync)
// {
//   int counter = 0;
//   {
//     boost::shared_ptr<TaskBase> task(new IncTask(counter));
//     LFixedThreadPool threadPool;
//     threadPool.AddTimerTask(task, 200);
//     ASSERT_EQ(0, counter);
//     task->CancelAsync();

//     MilliSleep(300);
//     ASSERT_EQ(0, counter);
//   }
//   ASSERT_EQ(0, counter);

//   {
//     boost::shared_ptr<TaskBase> task(new IncTask(counter));
//     LFixedThreadPool threadPool;
//     task = threadPool.AddTimerTask(task, 200);
//     ASSERT_EQ(0, counter);
//     task->CancelAsync();

//     MilliSleep(300);
//     ASSERT_EQ(0, counter);
//   }
//   ASSERT_EQ(0, counter);
// }

// TEST(FixedThreadPool, test_AddTimerTask_and_Cancel)
// {
//   int counter = 0;
//   {
//     boost::shared_ptr<TaskBase> task(new IncTask(counter));
//     LFixedThreadPool threadPool;
//     threadPool.AddTimerTask(task, 200);
//     ASSERT_EQ(0, counter);
//     task->Cancel();

//     MilliSleep(300);
//     ASSERT_EQ(0, counter);
//   }
//   ASSERT_EQ(0, counter);

//   {
//     boost::shared_ptr<TaskBase> task(new IncTask(counter));
//     LFixedThreadPool threadPool;
//     task = threadPool.AddTimerTask(task, 200);
//     ASSERT_EQ(0, counter);
//     task->Cancel();

//     MilliSleep(300);
//     ASSERT_EQ(0, counter);
//   }
//   ASSERT_EQ(0, counter);
// }


// TEST(FixedThreadPool, test_AddIntervalTask)
// {
//   int counter = 0;
//   {
//     LFixedThreadPool threadPool;
//     threadPool.AddIntervalTask(boost::shared_ptr<TaskBase>(new IncTask(counter)),
//         200, false);
//     ASSERT_EQ(0, counter);

//     MilliSleep(100);
//     ASSERT_EQ(0, counter);

//     MilliSleep(200);
//     ASSERT_EQ(1, counter);

//     MilliSleep(200);
//     ASSERT_EQ(2, counter);
//   }
//   MilliSleep(200);
//   ASSERT_EQ(2, counter);
// }

// TEST(FixedThreadPool, test_AddIntervalTask_and_CancelAsync)
// {
//   int counter = 0;
//   {
//     boost::shared_ptr<TaskBase> task(new IncTask(counter));
//     LFixedThreadPool threadPool;
//     threadPool.AddIntervalTask(task, 200, false);
//     ASSERT_EQ(0, counter);
//     task->CancelAsync();

//     MilliSleep(300);
//     ASSERT_EQ(0, counter);

//     MilliSleep(200);
//     ASSERT_EQ(0, counter);
//   }
//   ASSERT_EQ(0, counter);

//   {
//     boost::shared_ptr<TaskBase> task(new IncTask(counter));
//     LFixedThreadPool threadPool;
//     task = threadPool.AddIntervalTask(task, 200, false);
//     ASSERT_EQ(0, counter);
//     task->CancelAsync();

//     MilliSleep(300);
//     ASSERT_EQ(0, counter);

//     MilliSleep(200);
//     ASSERT_EQ(0, counter);
//   }
//   ASSERT_EQ(0, counter);
// }

// TEST(FixedThreadPool, test_AddIntervalTask_and_Cancel)
// {
//   int counter = 0;
//   {
//     boost::shared_ptr<TaskBase> task(new IncTask(counter));
//     LFixedThreadPool threadPool;
//     threadPool.AddIntervalTask(task, 200, false);
//     ASSERT_EQ(0, counter);
//     task->Cancel();

//     MilliSleep(300);
//     ASSERT_EQ(0, counter);

//     MilliSleep(200);
//     ASSERT_EQ(0, counter);
//   }
//   ASSERT_EQ(0, counter);

//   {
//     boost::shared_ptr<TaskBase> task(new IncTask(counter));
//     LFixedThreadPool threadPool;
//     task = threadPool.AddIntervalTask(task, 200, false);
//     ASSERT_EQ(0, counter);
//     task->Cancel();

//     MilliSleep(300);
//     ASSERT_EQ(0, counter);

//     MilliSleep(200);
//     ASSERT_EQ(0, counter);
//   }
//   ASSERT_EQ(0, counter);
// }

GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

