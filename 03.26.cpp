#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

void thfunc(int n, int m, int *k, char s[])
{
	this_thread::sleep_for(chrono::microseconds(1000));
	*k = 5000;
	cout << "in thfunc: n=" << n << ", m=" << m << ", k=" << *k << "\nstr=" << s << endl;
}

int main()
{
	int n = 110, m = 200, k = 5;
	char str[] = "hello world";

	thread t(thfunc, n, m, &k, str);
	t.detach();

	cout << "k=" << k << "\n";
	pthread_exit(NULL); // windows和linux表现不一样，windows子线程无输出，linux子线程有输出

	cout << "this line will not run" << endl;
	return 0;
}
