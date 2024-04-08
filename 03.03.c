#include <pthread.h>
#include <stdio.h>

void *thfunc(void *arg)
{
	//char *str = (char *)arg;
	const char *str = (const char *)arg;
	printf("in thfunc: str = %s\n", str);
	return NULL;
}

int main()
{
	const char *str = "hello world";

	pthread_t tidp;
	int ret = pthread_create(&tidp, NULL, thfunc, (void *)str);
	if (ret)
	{
		printf("pthread_create failed: %d\n", ret);
		return -1;
	}
	pthread_join(tidp, NULL);

	printf("in main: thread is created\n");

	return 0;
}
