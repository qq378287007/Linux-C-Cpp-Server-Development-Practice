#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

static int a = 200;
static int b = 100;

void *ThreadA(void *p)
{
	while (1)
	{
		a -= 50;
		b += 50;
	}
}

void *ThreadB(void *p)
{
	while (1)
	{
		printf("%d\n", a + b);
		sleep(1);
	}
}

int main()
{
	pthread_t tida, tidb;
	pthread_create(&tida, NULL, ThreadA, NULL);
	pthread_create(&tidb, NULL, ThreadB, NULL);
	pthread_join(tida, NULL);
	pthread_join(tidb, NULL);
	return 1;
}
