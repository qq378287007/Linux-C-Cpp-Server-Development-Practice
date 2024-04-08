#include <iostream>
#include <thread>
using namespace std;

thread::id main_thread_id = this_thread::get_id();

void is_main_thread()
{
	if (main_thread_id == this_thread::get_id())
		cout << "This is the main thread.\n";
	else
		cout << "This is not the main thread.\n";
}

int main()
{
	is_main_thread();

	thread th(is_main_thread);
	th.join();

	return 0;
}
