#include <pthread.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
using namespace std;

void *thfunc(void *arg)
{
	sleep(1);

	cout << ("sub thread is running\n");
	ofstream("tmp.file") << "out\n";

	exit(5);

	//return NULL;
	//return (void *)5;

	//pthread_exit((void *)5);
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

	pthread_attr_destroy(&thread_attr);
	
	pthread_exit(NULL);
	
	//return 0;
}
