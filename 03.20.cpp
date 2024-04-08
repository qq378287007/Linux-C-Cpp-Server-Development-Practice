#include <iostream>
#include <thread>
using namespace std;

void thfunc(int n)
{
	cout << "thfunc: " << n << endl;
}

int main()
{
	cout << "create 5 threads...\n";
	thread threads[5];
	for (int i = 0; i < 5; i++)
		threads[i] = thread(thfunc, i + 1);

	for (thread &t : threads)
		t.join();

	cout << "All threads joined.\n";

	return 0;
}
