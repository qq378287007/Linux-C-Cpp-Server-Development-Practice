
#include <time.h>
#include <stddef.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
using namespace std;

void getNowTime()
{
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	/*
	CLOCK_REALTIME:从UTC1970-1-1 0:0:0开始计时
	CLOCK_MONOTONIC:从系统启动这一刻起开始计时
　　 CLOCK_PROCESS_CPUTIME_ID:本进程到当前代码系统CPU花费的时间。
　　 CLOCK_THREAD_CPUTIME_ID:本线程到当前代码系统CPU花费的时间。
	*/

	// 计时时间转换为日历时间
	struct tm nowTime;
	localtime_r(&time.tv_sec, &nowTime);

	// char current[1024];
	printf("%04d-%02d-%02d %02d:%02d:%02d\n",
		   nowTime.tm_year + 1900,
		   nowTime.tm_mon + 1,
		   nowTime.tm_mday,
		   nowTime.tm_hour,
		   nowTime.tm_min,
		   nowTime.tm_sec);
}

int main()
{
	using chrono::system_clock;
	time_t tt = system_clock::to_time_t(system_clock::now());
	struct tm *ptm = localtime(&tt);

	getNowTime();

	cout << "Waiting for the next minute to begin...\n";
	++ptm->tm_min;
	ptm->tm_sec = 0;

	// mktime,把 timeptr 所指向的结构转换为自 1970 年 1 月 1 日以来持续时间的秒数
	this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));

	getNowTime();

	return 0;
}
