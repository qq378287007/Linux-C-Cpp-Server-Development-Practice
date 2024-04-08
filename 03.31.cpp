#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

int main()
{
	cout << "countdown:\n";
	for (int i = 5; i > 0; --i)
	{
		cout << i << endl;
		this_thread::sleep_for(chrono::milliseconds(1));
	}
	cout << "Lift off!\n";

	return 0;
}
