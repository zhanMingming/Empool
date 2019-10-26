/*
* @Author: zhanmingming
* @Date:   2019-10-26 21:28:23
* @Last Modified by:   zhanmingming
* @Last Modified time: 2019-10-26 21:33:30
*/

#include "ScheduledThreadPool.h"
#include <iostream>

using namespace zhanmm;
using namespace std;

void func() {
    std::cout << "func" << std::endl;
}


int main(void) {
    ScheduledThreadPool  t(4);
    t.AddCronTimerTask(func, 100);
}