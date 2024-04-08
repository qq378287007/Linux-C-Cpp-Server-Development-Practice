#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

volatile int counter(0);
mutex mtx;

void thrfunc()
{
	for (int i = 0; i < 10000; ++i)
	{
		if (mtx.try_lock())
		{
			++counter;
			mtx.unlock();
		}
		else
			cout << "try_lock false\n";
	}
}

int main()
{
	thread threads[10];

	for (int i = 0; i < 10; ++i)
		threads[i] = thread(thrfunc);

	for (thread &th : threads)
		th.join();

	cout << "count to " << counter << " successfully \n";

	return 0;
}
