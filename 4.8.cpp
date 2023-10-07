#include <cstdio>
#include <iostream>
#include <string.h>
using namespace std;

struct MyData
{
	int nLen;
	char data[0];
};

int main()
{
	char str[10] = "123456789";
	int nLen = sizeof(str);

	cout << "Size of MyData: " << sizeof(MyData) << endl;

	MyData *myData = (MyData *)malloc(sizeof(MyData) + nLen);

	myData->nLen = nLen;
	memcpy(myData->data, str, 10);

	cout << "myData's Data is: " << myData->data << endl;
	cout << "Size of MyData: " << sizeof(MyData) << endl;

	free(myData);

	return 0;
}
