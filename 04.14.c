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

typedef struct sockaddr SOCKADDR;

int main()
{
	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == -1)
	{
		printf("Error at socket()\n");
		return -1;
	}

	char on = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	char ip[] = "127.0.0.1";
	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(ip);
	service.sin_port = htons(9900);
	if (bind(s, (SOCKADDR *)&service, sizeof(service)) == -1)
	{
		printf("bind failed\n");
		return -1;
	}

	int optVal = 1;
	int optLen = sizeof(int);
	if (getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char *)&optVal, (socklen_t *)&optLen) == -1)
	{
		printf("getsockopt failed: %d", errno);
		return -1;
	}
	else
		printf("After bind, the value of SO_KEEPALIVE: %d\n", optVal);

	optVal = 1;
	if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char *)&optVal, optLen) != -1)
		printf("Successful activation of keep alive mechanism.\n");

	if (getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char *)&optVal, (socklen_t *)&optLen) == -1)
	{
		printf("getsockopt failed: %d", errno);
		return -1;
	}
	else
		printf("After setting, the value of SO_KEEPALIVE: %d\n", optVal);
	return 0;
}
