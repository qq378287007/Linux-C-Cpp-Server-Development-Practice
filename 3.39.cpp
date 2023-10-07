#include <iostream>
#include <thread>
#include <mutex>

volatile int counter(0);
std::mutex mtx;

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
			std::cout << "try_lock false\n";
	}
}

int main(int argc, const char *argv[])
{
	std::thread threads[10];

	for (int i = 0; i < 10; ++i)
		threads[i] = std::thread(thrfunc);

	for (auto &th : threads)
		th.join();

	std::cout << "count to " << counter << " successfully \n";

	return 0;
}
