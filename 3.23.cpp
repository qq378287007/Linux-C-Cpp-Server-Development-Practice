#include <iostream>
#include <thread>
using namespace std;

void thfunc(const char *s)
{
	cout << "thfunc: " << s << "\n";
}

int main()
{
	char s[] = "boy and girl";
	thread t(thfunc, s);
	t.join();

	return 0;
}
