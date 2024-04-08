#pragma once

class XTask
{
public:
	virtual ~XTask() {}
	virtual bool Init() = 0;

	struct event_base *base = 0; // 一客户端一个base
	int sock = 0;				 // 连接的sock
	int thread_id = 0;			 // 线程池id
};
