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
	service.sin_port = htons(8000);
	if (bind(s, (SOCKADDR *)&service, sizeof(service)) == -1)
	{
		printf("bind failed: %d\n", errno);
		return -1;
	}

	int optVal;
	int optLen = sizeof(optVal);
	if (getsockopt(s, SOL_SOCKET, SO_ACCEPTCONN, (char *)&optVal, (socklen_t *)&optLen) == -1)
		printf("getsockopt failed: %d", errno);
	else
		printf("Before listening, The value of SO_ACCEPTCONN:%d, The socket is not listening\n", optVal);

	if (listen(s, 100) == -1)
	{
		printf("listen failed:%d\n", errno);
		return -1;
	}

	if (getsockopt(s, SOL_SOCKET, SO_ACCEPTCONN, (char *)&optVal, (socklen_t *)&optLen) == -1)
	{
		printf("getsockopt failed:%d", errno);
		return -1;
	}
	else
		printf("After listening,The value of SO_ACCEPTCONN:%d, The socket is listening\n", optVal);
	return 0;
}
