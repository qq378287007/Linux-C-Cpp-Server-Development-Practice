#include <iostream>
#include <thread>
using namespace std;

void fun(int &n)
{
	cout << "fun: " << n << "\n";
	n += 20;
	this_thread::sleep_for(chrono::milliseconds(10));
}
int main()
{
	int n = 0;
	cout << "n=" << n << '\n';

	n = 10;
	thread t1(fun, ref(n));
	thread t2(move(t1));
	t2.join();

	cout << "n=" << n << '\n';
	return 0;
}
