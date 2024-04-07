#include <sys/types.h>
#include <event2/event-config.h>
#include <stdio.h>
#include <event.h>

struct event ev;
struct timeval tv = {3, 0};

void time_cb(int fd, short event, void *arg)
{
	printf("timer wakeup!\n");
	event_add(&ev, &tv);
}

int main()
{
	struct event_base *base = event_init();//初始化Reactor实例

	evtimer_set(&ev, time_cb, NULL);//初始化一个事件event
	//等价于event_set(&ev, -1, 0, time_cb, NULL);
	//void event_set(struct event *ev, int fd, short events, event_callback_fn cb, void *arg);
	//ev：初始化event事件对象
	//fd：绑定句柄，关注的信号
	//events：fd上关注事件类型，EV_READ | EV_WRITE | EV_PERSIST
	//cb：函数指针void (*)(int fd, short event, void *arg)
	//arg：传递给cb函数指针的参数

	event_base_set(base, &ev);//指明event注册到base实例

	event_add(&ev, &tv);//相当于调用Reactor::register_hangler()函数注册事件

	event_base_dispatch(base);//循环等待并处理事件

	return 0;
}

// sudo apt install libevent-dev
