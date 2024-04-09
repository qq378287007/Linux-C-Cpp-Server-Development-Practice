#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

int main()
{
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(8888);

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		puts("socket failed");
		return -1;
	}

	char on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	int val = sizeof(struct sockaddr);
	int ret = bind(sockfd, (struct sockaddr *)&saddr, val);
	if (ret < 0)
	{
		puts("sbind failed");
		return -1;
	}

	struct sockaddr_in raddr;
	char rbuf[50];
	char sbuf[100];
	while (1)
	{
		puts("waiting data");

		memset(rbuf, 0, 50);
		ret = recvfrom(sockfd, rbuf, 50, 0, (struct sockaddr *)&raddr, (socklen_t *)&val);
		if (ret < 0)
			perror("recvfrom failed");
		printf("recv data: %s\n", rbuf);

		memset(sbuf, 0, 100);
		sprintf(sbuf, "server has received your data(%s)\n", rbuf);
		ret = sendto(sockfd, sbuf, strlen(sbuf), 0, (struct sockaddr *)&raddr, sizeof(struct sockaddr));
	}

	close(sockfd);

	return 0;
}
