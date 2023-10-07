#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

int gcn = 0;

pthread_mutex_t mutex;

void *thread_1(void *arg)
{
	for (int j = 0; j < 10000000; j++)
	{
		pthread_mutex_lock(&mutex);
		gcn++;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit((void *)0);
}

void *thread_2(void *arg)
{
	for (int j = 0; j < 10000000; j++)
	{
		pthread_mutex_lock(&mutex);
		gcn++;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit((void *)0);
}

int main()
{
	int err;
	pthread_t th1, th2;

	pthread_mutex_init(&mutex, NULL);
	for (int j = 0; j < 10; j++)
	{
		gcn = 0;
		err = pthread_create(&th1, NULL, thread_1, (void *)0);
		if (err != 0)
		{
			printf("create new thread error:%s\n", strerror(err));
			exit(0);
		}
		err = pthread_create(&th2, NULL, thread_2, (void *)0);
		if (err != 0)
		{
			printf("create new thread error:%s\n", strerror(err));
			exit(0);
		}

		err = pthread_join(th1, NULL);
		if (err != 0)
		{
			printf("wait thread done error:%s\n", strerror(err));
			exit(1);
		}
		err = pthread_join(th2, NULL);
		if (err != 0)
		{
			printf("wait thread done error:%s\n", strerror(err));
			exit(1);
		}
		printf("gcn=%d\n", gcn);
	}
	pthread_mutex_destroy(&mutex);

	return 0;
}