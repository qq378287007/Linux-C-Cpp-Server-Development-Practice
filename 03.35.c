#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

static int a = 200;
static int b = 100;

pthread_mutex_t lock;

void *ThreadA(void *arg)
{
	while (1)
	{
		pthread_mutex_lock(&lock);
		a -= 50;
		b += 50;
		pthread_mutex_unlock(&lock);
	}
}

void *ThreadB(void *arg)
{
	while (1)
	{
		pthread_mutex_lock(&lock);
		printf("%d\n", a + b);
		pthread_mutex_unlock(&lock);
		sleep(1);
	}
}

int main()
{
	pthread_t tida, tidb;

	pthread_mutex_init(&lock, NULL);
	pthread_create(&tida, NULL, ThreadA, NULL);
	pthread_create(&tidb, NULL, ThreadB, NULL);
	pthread_join(tida, NULL);
	pthread_join(tidb, NULL);
	pthread_mutex_destroy(&lock);

	return 1;
}
