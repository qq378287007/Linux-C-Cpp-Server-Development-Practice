#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void mycleanfunc(void *arg)
{
	printf("mycleanfunc: %d\n", *(int *)arg);
}

void *thfunc(void *arg)
{
	printf("thread start-------- \n");
	int i = 1;
	pthread_cleanup_push(mycleanfunc, &i);
	while (1)
	{
		i++;
		printf("i=%d\n", i);
	}
	printf("this line will not run\n");
	pthread_cleanup_pop(0);

	return (void *)0;
}

int main()
{
	pthread_t tid;
	pthread_create(&tid, NULL, thfunc, NULL);
	sleep(1);

	pthread_cancel(tid);

	void *ret = NULL;
	pthread_join(tid, &ret);
	if (ret == PTHREAD_CANCELED)
		printf("thread has stopped,and exit code: %p\n", ret);
	else
		printf("some error occured");

	return 0;
}
