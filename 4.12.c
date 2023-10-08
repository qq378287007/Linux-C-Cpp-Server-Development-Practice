#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

int main()
{
	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == -1)
	{
		printf("Error at socket()\n");
		return -1;
	}
	
	int optVal;
	int optLen = sizeof(optVal);
	if (getsockopt(s, SOL_SOCKET, SO_TYPE, (char *)&optVal, (socklen_t *)&optLen) == -1)
		printf("getsockopt failed:%d", errno);
	else
	{
		if (SOCK_STREAM == optVal)
			printf("The current socket is a stream socket.\n");
		else if (SOCK_DGRAM == optVal)
			printf("The current socket is a datagram socket.\n");
	}

	int su = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (su == -1)
	{
		printf("Error at socket()\n");
		return -1;
	}
	if (getsockopt(su, SOL_SOCKET, SO_TYPE, (char *)&optVal, (socklen_t *)&optLen) == -1)
		printf("getsockopt failed:%d", errno);
	else
	{
		if (SOCK_STREAM == optVal)
			printf("The current socket is a stream socket.\n");
		else if (SOCK_DGRAM == optVal)
			printf("The current socket is a datagram socket.\n");
	}

	return 0;
}
