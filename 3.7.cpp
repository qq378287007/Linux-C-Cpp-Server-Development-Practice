#include <pthread.h>

#include <iostream>
using namespace std;

void *thfunc(void *arg)
{
	cout << "sub thread is running\n";
	return NULL;
}

int main(int argc, char *argv[])
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
	cout << "main thread will exit" << endl;

	pthread_exit(NULL);

	cout << "main thread has exited, this line will not run" << endl;
	return 0;
}
