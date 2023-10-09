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
	cout << "Size of MyData: " << sizeof(MyData) << endl;

	char str[10] = "123456";
	int nLen = sizeof(str);

	MyData *myData = (MyData *)malloc(sizeof(MyData) + nLen);

	myData->nLen = nLen;
	memcpy(myData->data, str, nLen);

	cout << "myData's Data is: " << myData->data << endl;
	cout << "Size of MyData: " << sizeof(MyData) << endl;

	free(myData);

	return 0;
}
