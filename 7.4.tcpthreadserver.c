#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

void *client_process(void *arg)
{
	int recv_len;
	char recv_buf[1024];
	int connfd = *(int *)arg;
	while ((recv_len = recv(connfd, recv_buf, sizeof(recv_buf) - 1, 0)) > 0)
	{
		recv_buf[recv_len] = 0;
		printf("recv_buf: %s\n", recv_buf);
		send(connfd, recv_buf, recv_len, 0);
	}
	printf("client closed!\n");
	close(connfd);
	return NULL;
}

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket error");
		exit(-1);
	}

	unsigned short port = 8888;
	struct sockaddr_in my_addr;
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_port = htons(port);
	printf("Binding server to port %d\n", port);

	char on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	int err_log = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
	if (err_log != 0)
	{
		perror("bind");
		close(sockfd);
		exit(-1);
	}

	err_log = listen(sockfd, 10);
	if (err_log != 0)
	{
		perror("listen");
		close(sockfd);
		exit(-1);
	}

	int connfd;
	while (1)
	{
		printf("Waiting client...\n");

		struct sockaddr_in client_addr;
		socklen_t cliaddr_len = sizeof(client_addr);
		connfd = accept(sockfd, (struct sockaddr *)&client_addr, &cliaddr_len);
		if (connfd < 0)
		{
			perror("accept this time");
			continue;
		}

		char cli_ip[INET_ADDRSTRLEN] = "";
		inet_ntop(AF_INET, &client_addr.sin_addr, cli_ip, INET_ADDRSTRLEN);
		printf("----------------------------------------------\n");
		printf("client ip=%s, port=%d\n", cli_ip, ntohs(client_addr.sin_port));
		if (connfd > 0)
		{
			pthread_t thread_id;
			pthread_create(&thread_id, NULL, client_process, (void *)&connfd);
			pthread_detach(thread_id);
		}
	}

	close(sockfd);

	return 0;
}
