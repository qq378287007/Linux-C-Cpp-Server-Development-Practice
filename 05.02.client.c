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
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddr.sin_port = htons(8888);

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("failed socket!");
		return -1;
	}

	char on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	int ret;
	char wbuf[50] = "write to udpserver";
	for (int i = 0; i < 5; i++)
	{
		puts("please enter data:");
		// memset(wbuf, 0, sizeof(wbuf));
		// scanf("%s", wbuf, sizeof(wbuf));
		ret = sendto(sockfd, wbuf, sizeof(wbuf), 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
		if (ret < 0)
			perror("sendto failed!");
	}

	ret = sendto(sockfd, NULL, 0, 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
	if (ret == 0)
		printf("sendto empty\n");

	close(sockfd);
	return 0;
}
