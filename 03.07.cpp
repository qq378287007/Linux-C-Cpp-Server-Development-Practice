#include <pthread.h>
#include <unistd.h>

#include <iostream>
using namespace std;

void *thfunc(void *arg)
{
	sleep(1);
	cout << "sub thread is running\n";
	//return NULL;
	return (void *)5;
	//exit(5);
}

int main()
{
	pthread_attr_t thread_attr;
	int res = pthread_attr_init(&thread_attr);
	if (res)
		cout << "pthread_attr_init failed:" << res << endl;

	res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if (res)
		cout << "pthread_attr_setdetachstate failed:" << res << endl;

	pthread_t thread_id;
	res = pthread_create(&thread_id, &thread_attr, thfunc, NULL);
	if (res)
		cout << "pthread_create failed:" << res << endl;

	pthread_attr_destroy(&thread_attr);

	cout << "main thread will exit\n";
	pthread_exit(NULL); // 主线程退出，进程不会退出，下面语句不会再执行。
	// 进程资源会为其它线程保持打开，直到其它线程（所有子线程）都终止。

	cout << "main thread has exited, this line will not run" << endl;
	return 0;
}