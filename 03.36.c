#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

static int gcn = 0;

pthread_mutex_t mutex;
pthread_rwlock_t rwlock;

void *thread_1(void *arg)
{
	volatile int a = 1;
	for (int j = 0; j < 100; j++)
	{
		pthread_mutex_lock(&mutex);
		// a = gcn;
		gcn = a;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}

void *thread_2(void *arg)
{
	volatile int b;
	for (int j = 0; j < 10000000; j++)
	{
		pthread_mutex_lock(&mutex);
		b = gcn;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit((void *)0);
}

void *thread_3(void *arg)
{
	volatile int a = 1;
	for (int j = 0; j < 100; j++)
	{
		// pthread_rwlock_rdlock(&rwlock);
		pthread_rwlock_wrlock(&rwlock);
		// a = gcn;
		gcn = a;
		pthread_rwlock_unlock(&rwlock);
	}
	pthread_exit(NULL);
}

void *thread_4(void *arg)
{
	volatile int b;
	for (int j = 0; j < 10000000; j++)
	{
		pthread_rwlock_rdlock(&rwlock);
		b = gcn;
		pthread_rwlock_unlock(&rwlock);
	}
	pthread_exit((void *)0);
}

int mutextVer()
{
	pthread_mutex_init(&mutex, NULL);

	struct timeval start;
	gettimeofday(&start, NULL);

	pthread_t th1, th2;
	int err = pthread_create(&th1, NULL, thread_1, (void *)0);
	if (err != 0)
	{
		printf("create new thread error: %s\n", strerror(err));
		exit(0);
	}
	err = pthread_create(&th2, NULL, thread_2, (void *)0);
	if (err != 0)
	{
		printf("create new thread error: %s\n", strerror(err));
		exit(0);
	}

	err = pthread_join(th1, NULL);
	if (err != 0)
	{
		printf("wait thread done error: %s\n", strerror(err));
		exit(1);
	}
	err = pthread_join(th2, NULL);
	if (err != 0)
	{
		printf("wait thread done error: %s\n", strerror(err));
		exit(1);
	}

	struct timeval end;
	gettimeofday(&end, NULL);

	pthread_mutex_destroy(&mutex);

	long long total_time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	total_time /= 1000;
	printf("total mutex time is %lld ms\n", total_time);

	return 0;
}

int rdlockVer()
{
	pthread_rwlock_init(&rwlock, NULL);

	struct timeval start;
	gettimeofday(&start, NULL);

	pthread_t th1, th2;
	int err = pthread_create(&th1, NULL, thread_3, (void *)0);
	if (err != 0)
	{
		printf("create new thread error:%s\n", strerror(err));
		exit(0);
	}
	err = pthread_create(&th2, NULL, thread_4, (void *)0);
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

	struct timeval end;
	gettimeofday(&end, NULL);

	pthread_rwlock_destroy(&rwlock);

	long long total_time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	total_time /= 1000;
	printf("total mutex time is %lld ms\n", total_time);

	return 0;
}

int main()
{
	mutextVer();
	rdlockVer();
	return 0;
}