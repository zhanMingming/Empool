## I.Introduction
Empool is a thread pool management library, which can create four different types of thread pools.

- Fixedthreadpool, Fixed size thread pool.

- Scalingthreadpool dynamic size thread pool, Dynamically increase or decrease the number of threads according to the busy degree of threads.

- Scheduledthreadpool, Fixed length thread pool, Supporting scheduled and periodic task execution.

- Singlethreadpool a one thread thread thread pool. The only working thread performs tasks. It supports three operation modes (FIFO, LIFO, priority).

For three modes of operation in singlethreadpool:



- FIFO: Indicates that the task will be executed in FIFO mode.

- LIFO: Perform tasks according to the mode of first in first out.

- Priority: Perform tasks according to their priority.


## II.Linux system installation

Empool depends on boost library 1.54.0+, GCC 4.8+, cmake 3.0+

### Installation dependency


###### Install compilation tools
```
yum install gcc-c++ make  git 

```

###### Install cmake version 3.12

```
#cmake ：https://cmake.org/files/

wget https://cmake.org/files/v3.12/cmake-3.12.0.tar.gz
tar xzvf cmake-3.12.0.tar.gz
cd cmake-3.12.0
./bootstrap
gmake
make install

```
###### Install boost version 1.71.0
```
wget https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.gz

tar -zxvf boost_1_71_0.tar.gz

cd boost_1_71_0 

./bootstrap.sh  --with-libraries=all && ./b2 && ./b2 isntall

```

### Install Empool

```
git clone https://github.com/zhanMingming/Empool.git

cd Empool/

mkdir build && cd build

cmake ..

make && make install

```


## III：Examples

For specific and complete examples, please refer to the example directory to provide four specific uses for creating thread pools. The following is only an overview of its use.


#### ScalingThreadPool Example


```
std::atomic<int>  a(0);

void func() {
    ++a;
}

#Initializing the scalingthreadpool with a range of 4-8 threads
ScalingThreadPool *scalingPool = ThreadPoolManager::newScalingThreadPool(4, 8);


# Add task
scalingPool->AddTask(func);

//Closing the thread pool blocking method waits for all threads to end
scalingPool->ShutDownNow();

//Non blocking, send a close signal to each thread to return

//scalingPool->ShutDown();

```
For the method of closing thread pool, all four thread pools are the same.

#### FixedThreadPool Example

```
void func() {
    do_something();
}

FixedThreadPool *fixedPool = ThreadPoolManager::newFixedThreadPool(8);

fixedPool->AddTask(func);
```

#### ScheduledThreadPool Example

```
void func() {
    do_something();
}

ScheduledThreadPool *scheduledPool = ThreadPoolManager::newScheduledThreadPool(4);

// Add a scheduled task, which will be executed in 100ms (once only)

scheduledPool->AddCronTimerTask(func, 100);

//  Add periodic tasks, and the time interval of task execution is 1000ms
scheduledPool->AddCycleTimerTask(func, 1000);

```
#### SingleThreadPool Example


```
// FIFO mode operation, the added tasks will be executed in FIFO mode.
SingleThreadPool *singlePool1 = ThreadPoolManager::newSingleThreadPool(FIFO);
singlePool1->AddTask(func);

//LIFO mode operation, the added tasks will be executed in the way of first in last out.
SingleThreadPool *singlePool2 = ThreadPoolManager::newSingleThreadPool(LIFO);
singlePool2->AddTask(func);

// The priority mode runs, and the task is executed according to the priority of the task.
SingleThreadPool *singlePool3 = ThreadPoolManager::newSingleThreadPool(PRIORITY);
singlePool3->AddTask(func, 100);

```
When using priority mode for SingleThreadPool, you need to specify the priority when adding tasks.
The rule here is that the smaller the number, the higher the priority.

