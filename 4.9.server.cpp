#include <cstdio>
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

#define BUF_LEN 300

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

struct MyData
{
	int nLen;
	char data[0];
};

int main()
{
	int sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(8000);

	bind(sockSrv, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
	listen(sockSrv, 5);

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	int cn = 5550;
	struct MyData *mydata;
	int iRes;
	char recvBuf[BUF_LEN];
	while (1)
	{
		printf("--------wait for client-----------\n");
		int sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, (socklen_t *)&len);
		printf("--------client comes-----------\n");

		mydata = (MyData *)malloc(sizeof(MyData) + cn);
		mydata->nLen = htonl(cn);
		memset(mydata->data, 'a', cn);
		mydata->data[cn - 1] = 'b';
		send(sockConn, (char *)mydata, sizeof(MyData) + cn, 0);
		free(mydata);

		do
		{
			iRes = recv(sockConn, recvBuf, BUF_LEN, 0);
			if (iRes > 0)
			{
				printf("\nRecv %d bytes: ", iRes);
				for (int i = 0; i < iRes; i++)
					printf("%c", recvBuf[i]);
				printf("\n");
			}
			else if (iRes == 0)
			{
				printf("\nThe client has closed the connection.\n");
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
