#pragma once
#include <event2/util.h>
#include <list>
#include <mutex>
class XTask;
struct event_base;
class XThread
{
public:
	// �����߳�
	void Start();

	// �߳���ں���
	void Main();

	// ��װ�̣߳���ʼ��evevnt_base�͹ܵ������¼����ڼ���
	bool Setup();

	// �յ����̷߳����ļ�����Ϣ���̳߳�����ַ���
	void Notify(evutil_socket_t, short);

	// �̼߳���
	void Activate();

	// ��������, һ���߳̿���ͬʱ����������񣬹���һ��event_base
	void AddTack(XTask *);

	XThread();
	~XThread();

	// �̱߳��
	int id = 0;

private:
	int notify_send_fd = 0;
	event_base *base = 0;
	std::list<XTask*> tasks;
	std::mutex mtx;
};

