#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include <iostream>
using namespace std;

static void on_signal_term(int sig) // 信号函数
{
	cout << "sub thread will exit" << endl;
	// pthread_exit(NULL);
	pthread_exit((void *)3);
}

void *thfunc(void *arg)
{
	signal(SIGQUIT, on_signal_term); // 注册信号函数

	int tm = 50;
	while (true)
	{
		cout << "thrfunc--left:" << tm << " s--" << endl;
		sleep(1); // 收到信号时，直接调用信号函数，退出线程
		tm--;
	}

	return (void *)10;
}

int main()
{
	pthread_t pid;
	int res = pthread_create(&pid, NULL, thfunc, NULL);

	sleep(5);

	pthread_kill(pid, SIGQUIT); // 向线程发送信号

	// pthread_exit((void *)5);

	pthread_join(pid, NULL);
	cout << "sub thread has completed, main thread will exit\n";

	return 0;
}