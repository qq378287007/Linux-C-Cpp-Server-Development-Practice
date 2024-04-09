#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define min(x, y)           \
	({                      \
		typeof(x) _x = (x); \
		typeof(y) _y = (y); \
		_x < _y ? _x : _y;  \
	})

#define BUF_LEN 250

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int main()
{
	int sockClient = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrSrv.sin_port = htons(8000);
	int err = connect(sockClient, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
	if (-1 == err)
	{
		printf("Failed to connect to the server:%d\n", errno);
		return -1;
	}

	char recvBuf[BUF_LEN];
	int iRes;
	int cn = 1;

	for (int leftlen = 50 * 111; leftlen > 0; leftlen -= iRes)
	{
		iRes = recv(sockClient, recvBuf, min(BUF_LEN, leftlen), 0);
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
			break;
		}
		else
		{
			printf("recv failed: %d\n", errno);
			close(sockClient);
			return -1;
		}
	}

	char sendBuf[100];
	memset(sendBuf, 0, sizeof(sendBuf));
	sprintf(sendBuf, "Hi, Server, I've finished receiving the data.");
	send(sockClient, sendBuf, strlen(sendBuf), 0);

	puts("Sending data to the server is completed");
	close(sockClient);
	return 0;
}
