#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <iostream>
#include <thread>
using namespace std;

void thfunc(int n)
{
	std::cout << "thfunc:" << n << endl;
}

int main(int argc, const char *argv[])
{
	std::cout << "create 5 threads...\n";
	std::thread threads[5];
	for (int i = 0; i < 5; i++)
		threads[i] = std::thread(thfunc, i + 1);

	for (auto &t : threads)
		t.join();

	std::cout << "All threads joined.\n";

	return EXIT_SUCCESS;
}
