#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

void mycleanfunc(void *arg)
{
	printf("mycleanfunc: %d\n", *(int *)arg);
}

void *thfrunc1(void *arg)
{
	int m = 1;
	printf("thfrunc1 comes \n");

	// push和pop必须配对
	pthread_cleanup_push(mycleanfunc, &m);
	return NULL; // 不执行清理函数
	pthread_cleanup_pop(0);
}

void *thfrunc2(void *arg)
{
	int m = 2;
	pthread_cleanup_push(mycleanfunc, &m);
	printf("thfrunc2 comes \n");
	pthread_exit(NULL); // 执行清理函数
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

	pthread_t pid2;
	res = pthread_create(&pid2, NULL, thfrunc2, NULL);
	if (res)
	{
		printf("pthread_create failed: %s\n", strerror(res));
		exit(1);
	}
	pthread_join(pid2, NULL);

	printf("main over\n");
	return 0;
}
