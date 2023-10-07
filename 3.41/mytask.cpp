#include "mytask.h"

bool CMyTask::Init(int arg) 
{
	long long i=0,c=0;
	while(c<10000000)
	{
		while(i<1000000000)
			i++;
		c++;
	}
	printf("%d---------%d--------\n",arg,c);
}
