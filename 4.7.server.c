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
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrSrv.sin_port = htons(8000);

	bind(sockSrv, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
	listen(sockSrv, 5);

	const int len = sizeof(SOCKADDR);

	SOCKADDR_IN addrClient;
	char sendBuf[111];
	char recvBuf[BUF_LEN];

	while (1)
	{
		printf("--------wait for client-----------\n");
		int sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, (socklen_t *)&len);
		printf("--------client comes-----------\n");
		memset(sendBuf, 'a', 111);
		for (int cn = 0; cn < 50; cn++)
		{
			if (cn == 49)
				sendBuf[110] = 'b';
			send(sockConn, sendBuf, 111, 0);
		}

		int iRes;
		do
		{
			iRes = recv(sockConn, recvBuf, BUF_LEN, 0);
			if (iRes > 0)
			{
				printf("\nRecv %d bytes:", iRes);
				for (int i = 0; i < iRes; i++)
					printf("%c", recvBuf[i]);
				printf("\n");
			}
			else if (iRes == 0)
			{
				printf("The client closes the connection.\n");
			}
			else
			{
				printf("recv failed with error: %d\n", errno);
				close(sockConn);
				// return 1;
				continue;
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
