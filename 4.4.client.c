#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main()
{
	int sockClient = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrSrv.sin_port = htons(8000);
	int err = connect(sockClient, (struct sockaddr *)&addrSrv, sizeof(struct sockaddr));
	if (-1 == err)
	{
		printf("Failed to connect to the server.Please check whether the server is started\n");
		return 0;
	}

	char recvBuf[100] = {0};
	recv(sockClient, recvBuf, 100, 0);
	printf("receive server's msg: %s\n", recvBuf);

	char msg[100] = "hi,server";
	// send(sockClient, msg, strlen(msg) + 1, 0);
	send(sockClient, msg, sizeof(msg), 0);

	close(sockClient);

	return 0;
}
