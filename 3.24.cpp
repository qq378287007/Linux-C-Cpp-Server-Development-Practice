#include <iostream>
#include <thread>
using namespace std;

typedef struct
{
	int n;
	const char *str;
} MYSTRUCT;

void thfunc(void *arg)
{
	MYSTRUCT *p = (MYSTRUCT *)arg;
	cout << "in thfunc:n=" << p->n << ", str=" << p->str << endl;
}

int main(int argc, char *argv[])
{
	MYSTRUCT mystruct = {110, "hello world"};

	thread t(thfunc, &mystruct);
	t.join();

	return 0;
}
