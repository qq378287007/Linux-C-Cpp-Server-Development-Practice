#include <iostream>
#include <thread>
using namespace std;

void th_function()
{
	cout << "i am c++11 thread func" << endl;
}

int main()
{
	thread t(th_function);
	t.join();

	return 0;
}