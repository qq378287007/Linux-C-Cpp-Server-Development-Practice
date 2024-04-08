#pragma once
#include <vector>
#include "XThread.h"
#include "XTask.h"
using namespace std;

class XThreadPool
{
public:
	static XThreadPool *Get() // 单例模式
	{
		static XThreadPool p;
		return &p;
	}

	void Init(int threadCount); // 初始化所有线程

	void Dispatch(XTask *); // 分发线程

private:
	XThreadPool(){};
	int threadCount;
	int lastThread = -1;
	vector<XThread *> threads;
};
