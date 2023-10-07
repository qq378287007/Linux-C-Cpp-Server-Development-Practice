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

unsigned long GetTickCount()
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) != 0)
		return 0;
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int main()
{
	char ip[] = "127.0.0.1";
	int port = 13334;
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	in_addr_t dwIP = inet_addr(ip);
	server_address.sin_addr.s_addr = dwIP;
	server_address.sin_port = htons(port);

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	assert(sock >= 0);

	long t1 = GetTickCount();
	int ret = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
	printf("connect ret code is: %d\n", ret);
	if (ret == -1)
	{
		long t2 = GetTickCount();
		printf("time used:%ldms\n", t2 - t1);
		printf("connect failed...\n");
		if (errno == EINPROGRESS)
			printf("unblock mode ret code...\n");
	}
	else
		printf("ret code is: %d\n", ret);

	int argp = 1;
	int res = ioctl(sock, FIONBIO, &argp);
	if (-1 == res)
	{
		printf("Error at ioctlsocket(): %d\n", errno);
		return -1;
	}
	puts("\nAfter setting non blocking mode:");
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	dwIP = inet_addr(ip);
	server_address.sin_addr.s_addr = dwIP;
	server_address.sin_port = htons(port);

	t1 = GetTickCount();
	ret = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
	printf("connect ret code is: %d\n", ret);
	if (ret == -1)
	{
		long t2 = GetTickCount();
		printf("time used:%ldms\n", t2 - t1);
		if (errno == EINPROGRESS)
			printf("unblock mode errno:%d\n", errno);
	}
	else
		printf("ret code is: %d\n", ret);

	close(sock);

	return 0;
}