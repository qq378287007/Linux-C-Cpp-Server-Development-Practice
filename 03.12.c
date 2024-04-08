#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define PTHREAD_NUM 2

void *thrfunc1(void *arg)
{
	static int count = 1;
	pthread_exit((void *)(&count));
}

void *thrfunc2(void *arg)
{
	static int count = 2;
	return (void *)(&count);
}

int main()
{
	pthread_t pid[PTHREAD_NUM] = {0};

	int retPid;
	if ((retPid = pthread_create(&pid[0], NULL, thrfunc1, NULL)) != 0)
	{
		perror("create pid first failed");
		return -1;
	}
	if ((retPid = pthread_create(&pid[1], NULL, thrfunc2, NULL)) != 0)
	{
		perror("create pid second failed");
		return -1;
	}

	int *pRet1;
	if (pid[0] != 0)
	{
		pthread_join(pid[0], (void **)&pRet1);
		printf("get thread 0 exitcode: %d\n", *pRet1);
	}

	int *pRet2;
	if (pid[1] != 0)
	{
		pthread_join(pid[1], (void **)&pRet2);
		printf("get thread 1 exitcode: %d\n", *pRet2);
	}
	return 0;
}
