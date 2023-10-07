#include <iostream>
#include <thread>
using namespace std;

void thfunc(int n, int m, int *k, char s[])
{
	*k = 5000;
	cout << "in thfunc:n=" << n << ", m=" << m << ", k=" << *k << "\nstr=" << s << endl;
}

int main(int argc, char *argv[])
{
	int n = 110, m = 200, k = 5;
	char str[] = "hello world";

	thread t(thfunc, n, m, &k, str); 
	t.detach();						 

	cout << "k=" << k << endl; 
	pthread_exit(NULL);		   

	cout << "this line will not run" << endl; 
	return 0;
}
