#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thfunc(void *arg)
{
	int i = 1;
	printf("thread start-------- \n");
	while (1)
		i++;
	printf("thread end-------- \n");

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
		printf("thread has stopped, and exit code: %d\n", *(int *)ret);
	else
		printf("some error occured");

	return 0;
}