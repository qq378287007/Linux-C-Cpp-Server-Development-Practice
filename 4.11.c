#include <stdio.h>
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

	int su = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == -1)
	{
		printf("Error at socket()\n");
		return -1;
	}

	int optVal;
	int optLen = sizeof(optVal);
	if (getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&optVal, (socklen_t *)&optLen) == -1)
		printf("getsockopt failed: %d", errno);
	else
		printf("Size of stream socket receive buffer: %d bytes\n", optVal);

	if (getsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&optVal, (socklen_t *)&optLen) == -1)
		printf("getsockopt failed: %d", errno);
	else
		printf("Size of streaming socket send buffer: %d bytes\n", optVal);

	if (getsockopt(su, SOL_SOCKET, SO_RCVBUF, (char *)&optVal, (socklen_t *)&optLen) == -1)
		printf("getsockopt failed: %d", errno);
	else
		printf("Size of datagram socket receive buffer: %d bytes\n", optVal);

	if (getsockopt(su, SOL_SOCKET, SO_SNDBUF, (char *)&optVal, (socklen_t *)&optLen) == -1)
		printf("getsockopt failed: %d", errno);
	else
		printf("Size of datagram socket send buffer: %d bytes\n", optVal);

	return 0;
}
