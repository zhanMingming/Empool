
#include "WorkerThread.h"
#include "BlockingTaskQueue.h"
//#include "EndTask.h"
#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <atomic>

using namespace zhanmm;
using namespace std;


namespace {
  struct EmptyTask: public TaskBase {

    virtual void DoRun() {}
  };

  struct TestTask : public TaskBase {
    atomic<int>& counter;

    TestTask(atomic<int>& i)
      : counter(i)
    {}

    virtual void DoRun()
    {
      sleep(2);
      ++counter;
    }
  };
}




// TEST(WorkerThread, test_Ctor)
// {
//   {

//     atomic<int> counter(0);
//     boost::shared_ptr<TaskQueueBase> q(new BlockingTaskQueue);
//     WorkerThread t(q);
//     q->Push(boost::shared_ptr<TaskBase>(new EmptyTask));
//   }
// }



TEST(WorkerThread, test_Close)
{
  atomic<int> counter(0);
  boost::shared_ptr<TaskQueueBase> q(new BlockingTaskQueue);
  {
    WorkerThread t(q);
    q->Push(boost::shared_ptr<TaskBase>(new TestTask(counter)));
    q->Push(boost::shared_ptr<TaskBase>(new TestTask(counter)));
    sleep(1);
    t.Close();
    // expect WorkerThread run only one task
    ASSERT_EQ(1, counter);
  }
  ASSERT_EQ(1, counter);
  ASSERT_EQ(static_cast<size_t>(1), q->Size());
}

TEST(WorkerThread, test_multiple_Close)
{
  atomic<int> counter(0);
  boost::shared_ptr<TaskQueueBase> q(new BlockingTaskQueue);
  {
    WorkerThread t(q);
    q->Push(boost::shared_ptr<TaskBase>(new TestTask(counter)));
    q->Push(boost::shared_ptr<TaskBase>(new TestTask(counter)));
    sleep(1);
    t.Close();
    t.Close(); // should not block the execution.
    // expect WorkerThread run only one task
  }
  ASSERT_EQ(1, counter);
}

namespace {
  struct CloseFunction {
    WorkerThread& m_workerThread;

    CloseFunction(WorkerThread& workerThread)
      : m_workerThread(workerThread)
    {}

    void operator()()
    {
      m_workerThread.Close();
    }
  };
}

TEST(WorkerThread, test_multiple_Close_simultunuously)
{
  atomic<int> counter(0);
  boost::shared_ptr<TaskQueueBase> q(new BlockingTaskQueue);
  {
    WorkerThread t(q);
    q->Push(boost::shared_ptr<TaskBase>(new TestTask(counter)));
    q->Push(boost::shared_ptr<TaskBase>(new TestTask(counter)));
    sleep(1);
    Thread t1((CloseFunction(t)));
    t.Close();
    // expect WorkerThread run only one task
  }
  ASSERT_EQ(1, counter);
}

// TEST(WorkerThread, test_CloseAsync)
// {
//   atomic<int> counter(0);
//   boost::shared_ptr<TaskQueueBase> q(new BlockingTaskQueue);
//   {
//     WorkerThread t(q);
//     q->Push(boost::shared_ptr<TaskBase>(new TestTask(counter)));
//     q->Push(boost::shared_ptr<TaskBase>(new TestTask(counter)));
//     sleep(1);
//     t.CloseAsync();
//     // expect WorkerThread finish zero task
//     ASSERT_EQ(0, counter);
//   }
// }

namespace {
  struct FinishAction
  {
    atomic<int>& counter;
    
    FinishAction(atomic<int>& i)
      : counter(i)
    {}

    void operator()(int threadId)
    {
      ++counter;
    }
  };
}

TEST(WorkerThread, test_ctor_with_FinishAction)
{
  atomic<int> counter(0);

  boost::shared_ptr<TaskQueueBase> q(new BlockingTaskQueue);
  {
    WorkerThread t(q, FinishAction(counter));
    q->Push(boost::shared_ptr<TaskBase>(new TestTask(counter)));
    sleep(1);
    t.Close();
  }
  ASSERT_EQ(2, counter);
}


namespace {
  struct LoopTask : TestTask {
    LoopTask(atomic<int>& i)
      : TestTask(i)
    {}

    virtual void DoRun()
    {
      for (int i = 0; i < 2; ++i)
    {
      TestTask::DoRun();
      //CheckCloselation();
    }
    }
  };
}

TEST(WorkerThread, test_CloseNow)
{
  atomic<int> counter(0);
  atomic<int> finishFlag(0);
  boost::shared_ptr<TaskQueueBase> q(new BlockingTaskQueue);
  {
    WorkerThread t(q, FinishAction(finishFlag));
    q->Push(boost::shared_ptr<TaskBase>(new LoopTask(counter)));
    q->Push(boost::shared_ptr<TaskBase>(new LoopTask(counter)));
    sleep(1);
    t.Close();
    //EXPECT_EQ(1, counter);
    EXPECT_EQ(1, finishFlag);
  }
}

TEST(WorkerThread, test_multiple_CloseNow)
{
  atomic<int> counter(0);
  atomic<int> finishFlag(0);
  boost::shared_ptr<TaskQueueBase> q(new BlockingTaskQueue);
  {
    WorkerThread t(q, FinishAction(finishFlag));
    q->Push(boost::shared_ptr<TaskBase>(new LoopTask(counter)));
    q->Push(boost::shared_ptr<TaskBase>(new LoopTask(counter)));
    sleep(1);
    t.Close();
    t.Close(); // should not block the execution.
    //EXPECT_EQ(1, counter);
    EXPECT_EQ(1, finishFlag);
  }
}

namespace {
  struct SleepFinishAction : public FinishAction {
    SleepFinishAction(atomic<int>& i)
      : FinishAction(i)
    {}
    
    void operator()()
    {
      sleep(1);
      FinishAction::operator()(1);
    }
  };
}

TEST(WorkerThread, test_FinishAcion_execute_before_Finished)
{
  atomic<int> counter(0);
  atomic<int> finishFlag(0);
  boost::shared_ptr<TaskQueueBase> q(new BlockingTaskQueue);
  {
    WorkerThread t(q, FinishAction(finishFlag));
    q->Push(boost::shared_ptr<TaskBase>(new LoopTask(counter)));
    sleep(1);
    t.Close();
    //EXPECT_EQ(1, counter);
    EXPECT_EQ(1, finishFlag);
  }
}

GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


