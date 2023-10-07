#pragma once

#include <list>
#include <mutex>
class XTask;
struct event_base;
class XThread
{
public:
	void Start();// 启动线程
	void Main();// 线程入口函数
	void Notify();// 收到主线程发出的激活消息（线程池任务分发）
	void Activate(int arg);// 线程激活
	void AddTack(XTask *);// 添加任务, 一个线程可以同时处理多个任务，共用一个event_base
	XThread();
	~XThread();
	int id = 0;// 线程编号

private:
	event_base *base = 0;
	std::list<XTask*> tasks;
	std::mutex tasks_mutex;
};

