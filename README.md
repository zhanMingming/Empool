## 一.介绍


Empool 是一个线程池管理库，可以创建以下四种不同类型的线程池。

- FixedThreadPool      固定大小线程池 
- ScalingThreadPool    动态大小线程池，根据线程的繁忙程度，动态增加或减少线程数量
- ScheduledThreadPool  定长线程池，支持定时及周期性任务执行
- SingleThreadPool    一个单线程化的线程池, 唯一的工作线程来执行任务，支持三种运行模式(FIFO、LIFO、PRIORITY)
```
对于 SingleThreadPool 中 三种运行模式：
FIFO：表明任务会按照先进先出的模式执行
LIFO：按照先进后出的模式执行任务
PRIORITY: 按照任务的优先级执行任务
```

## 二.Linux 系统下安装

Empool 依赖 Boost库1.54.0版本及以上、GCC 4.8及以上、CMake 3.0 版本及以上

### 安装依赖


###### 安装编译工具
```
yum install gcc-c++ make  git 

```

###### 安装 cmake 3.12版本

```
#cmake 提供的下载地址为：https://cmake.org/files/

wget https://cmake.org/files/v3.12/cmake-3.12.0.tar.gz
tar xzvf cmake-3.12.0.tar.gz
cd cmake-3.12.0
./bootstrap
gmake
make install

```
###### 安装 boost 1.71.0 版本
```
wget https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.gz

tar -zxvf boost_1_71_0.tar.gz

cd boost_1_71_0 

./bootstrap.sh  --with-libraries=all && ./b2 && ./b2 isntall

```

### 安装Empool

```
git clone https://github.com/zhanMingming/Empool.git

cd Empool/

mkdir build && cd build

cmake ..

make && make install

```

## 三：例子
具体的完整例子可以参考 example 目录下，提供四种创建线程池的具体用法，以下只是概述性地介绍其用法。


#### ScalingThreadPool 用法


一般用法：

```
std::atomic<int>  a(0);

void func() {
    ++a;
}

#初始化 ScalingThreadPool，线程数范围为4-8
ScalingThreadPool *scalingPool = ThreadPoolManager::newScalingThreadPool(4, 8);


# 添加Task任务
scalingPool->AddTask(func);

```

#### FixedThreadPool 用法

一般用法：
```
void func() {
    do_something();
}

FixedThreadPool *fixedPool = ThreadPoolManager::newFixedThreadPool(8);

fixedPool->AddTask(func);
```

#### ScheduledThreadPool 用法

一般用法：
```
void func() {
    do_something();
}

ScheduledThreadPool *scheduledPool = ThreadPoolManager::newScheduledThreadPool(4);

// 添加定时Task,任务将在100ms 之后执行(只执行一次)
scheduledPool->AddCronTimerTask(func, 100);

//  添加周期任务, 任务执行的时间间隔为1000ms
scheduledPool->AddCycleTimerTask(func, 1000);

```
#### SingleThreadPool 用法

一般用法：
```
// FIFO模式运行，添加的任务将会以先进先出的方式执行
SingleThreadPool *singlePool1 = ThreadPoolManager::newSingleThreadPool(FIFO);
singlePool1->AddTask(func);

//LIFO 模式运行，添加的任务将会以先进后出的方式执行
SingleThreadPool *singlePool2 = ThreadPoolManager::newSingleThreadPool(LIFO);
singlePool2->AddTask(func);

// PROIORITY 模式运行，根据任务的优先级执行任务
SingleThreadPool *singlePool3 = ThreadPoolManager::newSingleThreadPool(PRIORITY);
singlePool3->AddTask(func, 100);

```
对于SingleThreadPool 使用PRIORITY 模式时，需要在添加任务时指定优先级。
这里的规则是数字越小，优先级越高。


