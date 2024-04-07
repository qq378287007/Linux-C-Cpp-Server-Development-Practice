#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int i = 0;

int flag = 1;

void *thread1(void *junk)
{
	for (; i < 21; i++)
	{
		pthread_mutex_lock(&mutex);
		if (i % 3 == 0)
		{
			pthread_mutex_unlock(&mutex);
			pthread_cond_signal(&cond);
		}
		else
		{
			printf("thead1: %d\n", i);
			pthread_mutex_unlock(&mutex);
		}
		usleep(1000);
	}

	pthread_mutex_lock(&mutex);
	flag = 0;
	pthread_mutex_unlock(&mutex);

	pthread_cond_signal(&cond);
}

void *thread2(void *junk)
{
	while (flag)
	{
		pthread_mutex_lock(&mutex);

		if (i % 3 != 0)
			// while (i % 3 != 0)
			pthread_cond_wait(&cond, &mutex);

		printf("------------thread2: %d\n", i);
		pthread_mutex_unlock(&mutex);

		usleep(1000);
		// pthread_mutex_lock(&mutex);
		// i++;
		// pthread_mutex_unlock(&mutex);
	}
}

int main()
{
	pthread_t t_a;
	pthread_t t_b;

	pthread_create(&t_a, NULL, thread2, (void *)NULL);
	pthread_create(&t_b, NULL, thread1, (void *)NULL);
	pthread_join(t_b, NULL);
	pthread_join(t_a, NULL);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

	exit(0);
}
