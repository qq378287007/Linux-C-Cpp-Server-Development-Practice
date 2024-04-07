#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

void mycleanfunc(void *arg)
{
	printf("mycleanfunc: %d\n", *((int *)arg));
}

void *thfrunc1(void *arg)
{
	printf("thfrunc1 comes \n");
	int m = 1;
	pthread_cleanup_push(mycleanfunc, &m);

	int n = 2;
	pthread_cleanup_push(mycleanfunc, &n);
	pthread_cleanup_pop(1); // 弹出上一个入栈函数，并执行

	printf("thfrunc1 leaves \n");

	pthread_exit(NULL);
	// return NULL;

	pthread_cleanup_pop(0);
}

int main()
{
	pthread_t pid1;
	int res = pthread_create(&pid1, NULL, thfrunc1, NULL);
	if (res)
	{
		printf("pthread_create failed: %s\n", strerror(res));
		exit(1);
	}
	pthread_join(pid1, NULL);

	printf("main over\n");
	return 0;
}