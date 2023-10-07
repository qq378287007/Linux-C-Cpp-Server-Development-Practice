
#include <iostream>
#include <thread>
#include <atomic>
using namespace std;

atomic<bool> ready(false);

void thfunc(int id)
{
	while (!ready)
		this_thread::yield();

	for (volatile int i = 0; i < 1000000; ++i)
	{
	}
	cout << id << ",";
}

int main()
{
	cout << "race of 10 threads that count to 1 million:\n";

	thread threads[10];
	for (int i = 0; i < 10; ++i)
		threads[i] = thread(thfunc, i);
	ready = true;
	for (auto &th : threads)
		th.join();

	cout << '\n';
	return 0;
}