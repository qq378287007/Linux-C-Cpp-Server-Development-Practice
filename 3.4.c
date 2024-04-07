#include <pthread.h>
#include <stdio.h>

typedef struct // 定义结构体的类型
{
	int n;
	const char *str;
} MYSTRUCT;

void *thfunc(void *arg)
{
	MYSTRUCT *p = (MYSTRUCT *)arg;
	printf("in thfunc: n=%d, str=%s\n", p->n, p->str); // 打印结构体的内容
	return (void *)0;
}

int main()
{
	MYSTRUCT mystruct = {110, "hello world"};

	pthread_t tidp;
	int ret = pthread_create(&tidp, NULL, thfunc, (void *)&mystruct); // 创建线程并传递结构体地址
	if (ret)
	{
		printf("pthread_create failed:%d\n", ret);
		return -1;
	}
	pthread_join(tidp, NULL); // 等待子线程结束

	printf("in main: thread is created\n");

	return 0;
}
