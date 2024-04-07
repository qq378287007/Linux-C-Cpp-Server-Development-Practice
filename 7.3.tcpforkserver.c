#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// #include <ws2tcpip.h>

int main()
{
	unsigned short port = 8888;

	struct sockaddr_in my_addr;
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// inet_pton(AF_INET, "127.0.0.1", &my_addr.sin_addr);
	my_addr.sin_port = htons(port);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		exit(-1);
	}

	char on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	int err_log = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
	if (err_log != 0)
	{
		perror("binding");
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

	socklen_t cliaddr_len = sizeof(struct sockaddr_in);
	while (1)
	{
		puts("Father process is waitting client...");

		struct sockaddr_in client_addr;
		int connfd = accept(sockfd, (struct sockaddr *)&client_addr, &cliaddr_len);
		if (connfd < 0)
		{
			perror("accept");
			close(sockfd);
			exit(-1);
		}

		pid_t pid = fork();
		if (pid < 0)
		{
			perror("fork");
			_exit(-1);
		}
		else if (0 == pid)
		{
			close(sockfd);

			/*
			INT WSAAPI inet_pton(
			  INT   Family,  //地址家族  IPV4使用AF_INET  IPV6使用AF_INET6
			  PCSTR pszAddrString, //指向以NULL为结尾的字符串指针，该字符串包含要转换为数字的二进制形式的IP地址文本形式。
			  PVOID pAddrBuf//指向存储二进制表达式的缓冲区
			);
			*/
			/*
			PCWSTR WSAAPI InetNtopW(
				INT        Family,  //地址家族  IPV4使用AF_INET  IPV6使用AF_INET6
				const VOID *pAddr,  //指向网络字节中要转换为字符串的IP地址的指针
				PWSTR      pStringBuf,//指向缓冲区的指针，该缓冲区用于存储IP地址的以NULL终止的字符串表示形式。
				size_t     StringBufSize//输入时，由pStringBuf参数指向的缓冲区的长度（以字符为单位）
			);
			*/
			char cli_ip[INET_ADDRSTRLEN] = {0};
			memset(cli_ip, 0, sizeof(cli_ip));
			inet_ntop(AF_INET, &client_addr.sin_addr, cli_ip, INET_ADDRSTRLEN);
			printf("----------------------------------------------\n");
			printf("client ip=%s, port=%d\n", cli_ip, ntohs(client_addr.sin_port));

			char recv_buf[1024];
			int recv_len = 0;
			while ((recv_len = recv(connfd, recv_buf, sizeof(recv_buf) - 1, 0)) > 0)
			{
				recv_buf[recv_len] = 0;
				printf("recv_buf: %s\n", recv_buf);
				send(connfd, recv_buf, recv_len, 0);
			}

			printf("client_port %d closed!\n", ntohs(client_addr.sin_port));
			close(connfd);
			exit(0);
		}
		else
			close(connfd);
	}

	close(sockfd);

	return 0;
}