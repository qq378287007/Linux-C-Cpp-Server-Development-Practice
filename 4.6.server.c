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
	int sockSrv = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockSrv >= 0);

	char on = 1;
	setsockopt(sockSrv, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(8000);

	bind(sockSrv, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
	listen(sockSrv, 5);

	const int len = sizeof(SOCKADDR);

	SOCKADDR_IN addrClient;

	while (1)
	{
		printf("--------wait for client-----------\n");

		int sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, (socklen_t *)&len);
		char sendBuf[100] = "";
		for (int i = 0; i < 10; i++)
		{
			memset(sendBuf, 0, sizeof(sendBuf));
			sprintf(sendBuf, "N0.%d Welcome to the server. What is 1 + 1 = ? (client IP: %s, client Port: %d)\n", i + 1, inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
			send(sockConn, sendBuf, strlen(sendBuf), 0);
		}

		int iRes = shutdown(sockConn, SHUT_WR);
		if (iRes == -1)
		{
			printf("shutdown failed with error: %d\n", errno);
			close(sockConn);
			return 1;
		}

		char recvBuf[BUF_LEN];
		do
		{
			iRes = recv(sockConn, recvBuf, BUF_LEN, 0);
			if (iRes > 0)
			{
				printf("Recv %d bytes.\n", iRes);
				for (int i = 0; i < iRes; i++)
					printf("%c", recvBuf[i]);
				printf("\n");
			}
			else if (iRes == 0)
			{
				printf("The client has closed the connection.\n");
			}
			else
			{
				printf("recv failed with error: %d\n", errno);
				close(sockConn);
				return 1;
			}

		} while (iRes > 0);

		close(sockConn);
		/*
		puts("Continue monitoring?(y/n)");
		char ch[2];
		scanf("%s", ch, 2);
		if (ch[0] != 'y')
			break;
		*/
	}

	close(sockSrv);

	return 0;
}
