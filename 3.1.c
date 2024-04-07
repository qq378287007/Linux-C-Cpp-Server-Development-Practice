#include <pthread.h>
#include <stdio.h>
#include <unistd.h> //sleep

void *thfunc(void *arg)
{
	printf("in thfunc\n");
	return NULL;
}

int main()
{
	pthread_t tidp;
	int ret = pthread_create(&tidp, NULL, thfunc, NULL);
	if (ret)
	{
		printf("pthread_create failed: %d\n", ret);
		return -1;
	}

	sleep(1);

	printf("in main: thread is created\n");

	return 0;
}
