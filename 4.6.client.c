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

#define BUF_LEN 300

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
		printf("Failed to connect to the server. Please check whether the server is started\n");
		return 0;
	}

	char recvBuf[BUF_LEN];
	int iRes;
	do
	{
		iRes = recv(sockClient, recvBuf, BUF_LEN, 0);
		if (iRes > 0)
		{
			printf("\nRecv %d bytes:", iRes);
			for (int i = 0; i < iRes; i++)
				printf("%c", recvBuf[i]);
			printf("\n");
		}
		else if (iRes == 0)
		{
			puts("The server has closed the send connection.\n");
		}
		else
		{
			printf("recv failed:%d\n", errno);
			close(sockClient);
			return 1;
		}
	} while (iRes > 0);

	char sendBuf[100];
	for (int i = 0; i < 10; i++)
	{
		memset(sendBuf, 0, sizeof(sendBuf));
		sprintf(sendBuf, "N0.%d I'm the client, 1+1=2\n", i + 1);
		send(sockClient, sendBuf, strlen(sendBuf) + 1, 0);
	}
	puts("Sending data to the server is completed.");

	close(sockClient);

	return 0;
}
