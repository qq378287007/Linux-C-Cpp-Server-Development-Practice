#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main()
{
	int sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	char on = 1;
	setsockopt(sockSrv, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	struct sockaddr_in addrSrv;
	memset(&addrSrv, 0, sizeof(struct sockaddr_in));
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrSrv.sin_port = htons(8000);

	if (-1 == bind(sockSrv, (struct sockaddr *)&addrSrv, sizeof(struct sockaddr)))
	{
		printf("bind() fail: %d!\n", errno);
		return -1;
	}

	const int len = sizeof(struct sockaddr_in);

	struct sockaddr_in serv;
	getsockname(sockSrv, (struct sockaddr *)&serv, (socklen_t *)&len);
	printf("server has started, ip=%s, port=%d\n", inet_ntoa(serv.sin_addr), ntohs(serv.sin_port));

	listen(sockSrv, 5);

	struct sockaddr_in addrClient;
	while (1)
	{
		printf("--------wait for client-----------\n");
		int sockConn = accept(sockSrv, (struct sockaddr *)&addrClient, (socklen_t *)&len);

		char sendBuf[100] = {0};
		sprintf(sendBuf, "Welcome client(%s: %d) to Server!", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
		// send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
		send(sockConn, sendBuf, sizeof(sendBuf), 0);

		char recvBuf[100];
		recv(sockConn, recvBuf, 100, 0);
		printf("Receive client's msg: %s\n", recvBuf);

		close(sockConn);
		/*
		puts("continue to listen?(y/n)");
		char ch[2];
		scanf("%s", ch, 2);

		if (ch[0] != 'y')
			break;
		*/
	}

	close(sockSrv);

	return 0;
}
