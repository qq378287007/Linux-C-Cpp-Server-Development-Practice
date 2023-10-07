#pragma once

#include <list>
#include <mutex>
class XTask;
struct event_base;
class XThread
{
public:
	void Start();// �����߳�
	void Main();// �߳���ں���
	void Notify();// �յ����̷߳����ļ�����Ϣ���̳߳�����ַ���
	void Activate(int arg);// �̼߳���
	void AddTack(XTask *);// ��������, һ���߳̿���ͬʱ����������񣬹���һ��event_base
	XThread();
	~XThread();
	int id = 0;// �̱߳��

private:
	event_base *base = 0;
	std::list<XTask*> tasks;
	std::mutex mtx;
};

