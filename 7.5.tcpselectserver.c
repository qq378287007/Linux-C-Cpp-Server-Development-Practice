#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MYPORT 8888
#define MAXCLINE 5
#define BUF_SIZE 200

int conn_amount = 0;
int fd[MAXCLINE] = {0};

void showclient()
{
	printf("client amount: %d\n", conn_amount);
	for (int i = 0; i < MAXCLINE; i++)
		printf("[%d]: %d ", i, fd[i]);
	printf("\n\n");
}

int main()
{
	int sock_fd;
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}

	int yes = 1;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("setsockopt error \n");
		exit(1);
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(MYPORT);
	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind error!\n");
		close(sock_fd);
		exit(1);
	}

	if (listen(sock_fd, MAXCLINE) == -1)
	{
		perror("listen error!\n");
		close(sock_fd);
		exit(1);
	}
	printf("listen port %d\n", MYPORT);

	fd_set fdsr;
	int maxsock = sock_fd;
	struct timeval tv = {30, 0};
	int ret;
	int new_fd;
	struct sockaddr_in client_addr;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	char buf[BUF_SIZE];
	while (1)
	{
		FD_ZERO(&fdsr);
		FD_SET(sock_fd, &fdsr); // 监听套接字

		for (int i = 0; i < MAXCLINE; i++)
			if (fd[i] != 0)
				FD_SET(fd[i], &fdsr); // 连接套接字

		ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
		if (ret < 0)
		{
			perror("select error!\n");
			break;
		}
		else if (ret == 0)
		{
			printf("timeout\n");
			continue;
		}

		for (int i = 0; i < conn_amount; i++)
		{
			if (FD_ISSET(fd[i], &fdsr))
			{
				ret = recv(fd[i], buf, sizeof(buf), 0);
				if (ret <= 0)
				{
					printf("client[%d] close\n", i);
					close(fd[i]);
					FD_CLR(fd[i], &fdsr);
					fd[i] = 0;
					conn_amount--;
				}
				else
				{
					if (ret < BUF_SIZE)
					{
						memset(&buf[ret], '\0', 1);
						ret += 1;
					}
					printf("client[%d] send: %s\n", i, buf);
					send(fd[i], buf, ret, 0);
				}
			}
		}

		if (FD_ISSET(sock_fd, &fdsr))
		{
			new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
			if (new_fd <= 0)
			{
				perror("accept error\n");
				continue;
			}

			if (conn_amount < MAXCLINE)
			{
				for (int i = 0; i < MAXCLINE; i++)
				{
					if (fd[i] == 0)
					{
						fd[i] = new_fd;
						break;
					}
				}
				conn_amount++;
				printf("new connection client[%d] %s:%d\n", conn_amount, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
				if (new_fd > maxsock)
					maxsock = new_fd;
			}
			else
			{
				printf("max connections arrive, exit\n");
				send(new_fd, "bye", 4, 0);
				close(new_fd);
				continue;
			}
		}

		showclient();
	}

	for (int i = 0; i < MAXCLINE; i++)
		if (fd[i] != 0)
			close(fd[i]);
	close(sock_fd);

	return 0;
}