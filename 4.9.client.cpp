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
#include <malloc.h>

#define BUF_LEN 250

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int main()
{
	int sockClient = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(8000);
	int err = connect(sockClient, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
	if (-1 == err)
	{
		printf("Failed to connect to the server:%d\n", errno);
		return 0;
	}

	int leftlen;
	int iRes = recv(sockClient, (char *)&leftlen, sizeof(int), 0);
	leftlen = ntohl(leftlen);
	printf("Need to receive %d bytes data.\n", leftlen);

	char recvBuf[BUF_LEN];
	int cn = 1;

	for (; leftlen > 0; leftlen -= iRes)
	{
		if (leftlen < BUF_LEN)
			iRes = recv(sockClient, recvBuf, leftlen, 0);
		else
			iRes = recv(sockClient, recvBuf, BUF_LEN, 0);
			
		if (iRes > 0)
		{
			printf("\nNo.%d: Recv %d bytes: ", cn++, iRes);
			for (int i = 0; i < iRes; i++)
				printf("%c", recvBuf[i]);
			printf("\n");
		}
		else if (iRes == 0)
		{
			puts("\nThe server has closed the send connection.\n");
		}
		else
		{
			printf("recv failed: %d\n", errno);
			close(sockClient);
			return -1;
		}
	}

	/*
	while (leftlen > BUF_LEN)
	{
		iRes = recv(sockClient, recvBuf, BUF_LEN, 0);
		if (iRes > 0)
		{
			printf("\nNo.%d: Recv %d bytes: ", cn++, iRes);
			for (int i = 0; i < iRes; i++)
				printf("%c", recvBuf[i]);
			printf("\n");
		}
		else if (iRes == 0)
		{
			puts("\nThe server has closed the send connection.\n");
		}
		else
		{
			printf("recv failed: %d\n", errno);
			close(sockClient);
			return -1;
		}
		leftlen -= iRes;
	}
	if (leftlen > 0)
	{
		iRes = recv(sockClient, recvBuf, leftlen, 0);
		if (iRes > 0)
		{
			printf("\nNo.%d: Recv %d bytes: ", cn++, iRes);
			for (int i = 0; i < iRes; i++)
				printf("%c", recvBuf[i]);
			printf("\n");
		}
		else if (iRes == 0)
		{
			puts("\nThe server has closed the send connection.\n");
		}
		else
		{
			printf("recv failed: %d\n", errno);
			close(sockClient);
			return -1;
		}
		leftlen -= iRes;
	}
	*/

	char sendBuf[100];
	memset(sendBuf, 0, sizeof(sendBuf));
	sprintf(sendBuf, "I'm the client. I've finished receiving the data.");
	send(sockClient, sendBuf, strlen(sendBuf) + 1, 0);

	puts("Sending data to the server is completed");
	close(sockClient);
	return 0;
}
