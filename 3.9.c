#ifndef _GNU_SOURCE
#define _GNU_SOURCE // To get pthread_getattr_np() declaration
#endif

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

static void *thread_start(void *arg)
{
	pthread_attr_t gattr;
	int s = pthread_getattr_np(pthread_self(), &gattr);
	if (s != 0)
		printf("pthread_getattr_np failed\n");

	int i;
	s = pthread_attr_getdetachstate(&gattr, &i);
	if (s)
		printf("pthread_attr_getdetachstate failed");
	printf("Detach state = %s\n", (i == PTHREAD_CREATE_DETACHED)   ? "PTHREAD_CREATE_DETACHED"
								  : (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE"
																   : "???");
	pthread_detach(pthread_self());

	s = pthread_getattr_np(pthread_self(), &gattr);
	if (s != 0)
		printf("pthread_getattr_np failed\n");
	s = pthread_attr_getdetachstate(&gattr, &i);
	if (s)
		printf(" pthread_attr_getdetachstate failed");
	printf("after pthread_detach\nDetach state = %s\n", (i == PTHREAD_CREATE_DETACHED)	 ? "PTHREAD_CREATE_DETACHED"
														: (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE"
																						 : "???");
	pthread_attr_destroy(&gattr);
	return NULL;
}

int main()
{
	pthread_t thread_id;
	int s = pthread_create(&thread_id, NULL, &thread_start, NULL);
	if (s != 0)
	{
		printf("pthread_create failed\n");
		return s;
	}
	pthread_exit(NULL);
}
