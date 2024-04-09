#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>

#define MAXLINE 80
#define SERV_PORT 8888
#define OPEN_MAX 1024

int main()
{
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
		perror("bind");
	puts("bind ok");
	if (-1 == listen(listenfd, 20))
		perror("listen");
	puts("listen ok");

	int client[OPEN_MAX];
	for (int i = 0; i < OPEN_MAX; i++)
		client[i] = -1;
	int maxi = -1;
	int efd = epoll_create(OPEN_MAX);
	if (efd == -1)
		perror("epoll_create");

	struct epoll_event tep;
	tep.events = EPOLLIN;
	tep.data.fd = listenfd;
	int res = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tep);
	if (res == -1)
		perror("epoll_ctl");

	struct epoll_event ep[OPEN_MAX];
	while (1)
	{
		int nready = epoll_wait(efd, ep, OPEN_MAX, -1);
		if (nready == -1)
			perror("epoll_wait");

		for (int i = 0; i < nready; i++)
		{
			if (!(ep[i].events & EPOLLIN))
				continue;

			if (ep[i].data.fd == listenfd)
			{
				struct sockaddr_in cliaddr;
				socklen_t clilen = sizeof(cliaddr);
				int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

				char str[INET_ADDRSTRLEN];
				printf("received from %s at PORT %d\n",
					   inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));

				int j;
				for (j = 0; j < OPEN_MAX; j++)
				{
					if (client[j] < 0)
					{
						client[j] = connfd;
						break;
					}
				}

				if (j == OPEN_MAX)
					perror("too many clients");

				if (j > maxi)
					maxi = j;

				tep.events = EPOLLIN;
				tep.data.fd = connfd;
				res = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &tep);
				if (res == -1)
					perror("epoll_ctl");
			}
			else
			{
				int sockfd = ep[i].data.fd;
				char buf[MAXLINE];
				ssize_t n = read(sockfd, buf, MAXLINE);
				if (n == 0)
				{
					int j;
					for (j = 0; j <= maxi; j++)
					{
						if (client[j] == sockfd)
						{
							client[j] = -1;
							break;
						}
					}
					res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
					if (res == -1)
						perror("epoll_ctl");
					close(sockfd);
					printf("client[%d] closed connection\n", j);
				}
				else
				{
					printf("recive client's data: %s\n", buf);
					for (int j = 0; j < n; j++)
						buf[j] = toupper(buf[j]);
					write(sockfd, buf, n);
				}
			}
		}
	}

	close(efd);
	close(listenfd);

	return 0;
}
