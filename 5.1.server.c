#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main()
{
	int size = sizeof(struct sockaddr_in);

	struct sockaddr_in saddr;
	memset(&saddr, 0, size);
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddr.sin_port = htons(9999);

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("socket() failed");
		return -1;
	}

	char on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	int ret = bind(sockfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
	if (ret < 0)
	{
		perror("bind() failed");
		return -1;
	}

	int val = sizeof(struct sockaddr);
	puts("waiting data");
	struct sockaddr_in raddr;
	char rbuf[50];
	ret = recvfrom(sockfd, rbuf, 50, 0, (struct sockaddr *)&raddr, (socklen_t *)&val);
	if (ret < 0)
		perror("recvfrom failed");
	printf("recv data: %s\n", rbuf);
	
	close(sockfd);
	return 0;
}
