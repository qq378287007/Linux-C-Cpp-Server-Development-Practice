#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include "mydb.h"

#define MAXLINE 80
#define SERV_PORT 8000

#define CL_CMD_REG 'r'
#define CL_CMD_LOGIN 'l'
#define CL_CMD_CHAT 'c'

int GetName(char *str, char *szName)
{
	int e_ind = strlen(str) - 1;
	int s_ind = e_ind + 1;

	for (int i = 0; i <= e_ind; i++)
	{
		if (str[i] == ',')
		{
			s_ind = i + 1;
			break;
		}
	}

	for (int i = s_ind; i <= e_ind; i++)
	{
		if (str[i] == ',')
		{
			e_ind = i - 1;
			break;
		}
	}

	for (int i = s_ind; i <= e_ind; i++)
		szName[i - s_ind] = str[i];
}

// 查找字符串中某个字符出现的次数
int countChar(const char *p, const char chr)
{
	int count = 0;
	for (int i = 0; p[i]; i++)
		if (p[i] == chr)
			++count;
	return count;
}

int main()
{
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	// 创建套接字
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);

	int val = 1;
	int ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(int));

	// 绑定
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	// 监听
	listen(listenfd, 20); // 默认最大128

	// 需要接收最大文件描述符
	int maxfd = listenfd;

	// 数组初始化为-1
	int maxi = -1;
	int client[FD_SETSIZE];
	for (int i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;

	// 集合清零
	fd_set allset;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset); // 将listenfd加入allset集合

	puts("Chat server is running...");

	while (1)
	{
		// 关键点3
		fd_set rset = allset; /* 每次循环时都重新设置select监控信号集 */

		// select返回rest集合中发生读事件的总数  参数1：最大文件描述符+1
		int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
		if (nready < 0)
			puts("select error");

		// listenfd是否在rset集合中
		if (FD_ISSET(listenfd, &rset))
		{
			struct sockaddr_in cliaddr;
			// accept接收
			socklen_t cliaddr_len = sizeof(cliaddr);
			// accept返回通信套接字，当前非阻塞，因为select已经发生读写事件
			int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

			char str[INET_ADDRSTRLEN]; /* #define INET_ADDRSTRLEN 16 */
			printf("received from %s at PORT %d\n",
				   inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
				   ntohs(cliaddr.sin_port));

			// 关键点1
			int i;
			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (client[i] < 0)
				{
					client[i] = connfd; /* 保存accept返回的通信套接字connfd存到client[]里 */
					break;
				}
			}

			/* 是否达到select能监控的文件个数上限 1024 */
			if (i == FD_SETSIZE)
			{
				fputs("too many clients\n", stderr);
				exit(1);
			}

			// 关键点2
			FD_SET(connfd, &allset); /*添加一个新的文件描述符到监控信号集里 */

			// 更新最大文件描述符数
			if (connfd > maxfd)
				maxfd = connfd; /* select第一个参数需要 */
			if (i > maxi)
				maxi = i; /* 更新client[]最大下标值 */

			/* 如果没有更多的就绪文件描述符继续回到上面select阻塞监听,负责处理未处理完的就绪文件描述符 */
			if (--nready == 0)
				continue;
		}

		for (int i = 0; i <= maxi; i++)
		{
			// 检测clients 哪个有数据就绪
			int sockfd;
			if ((sockfd = client[i]) < 0)
				continue;

			// sockfd（connd）是否在rset集合中
			if (FD_ISSET(sockfd, &rset))
			{
				ssize_t n;
				char buf[MAXLINE];
				// 进行读数据 不用阻塞立即读取（select已经帮忙处理阻塞环节）
				if ((n = read(sockfd, buf, MAXLINE)) == 0)
				{
					/* 无数据情况 client关闭链接，服务器端也关闭对应链接 */
					close(sockfd);
					FD_CLR(sockfd, &allset); /*解除select监控此文件描述符 */
					client[i] = -1;
				}
				else
				{
					char szName[255] = "";
					char repBuf[512] = "";

					char code = buf[0];
					switch (code)
					{
					case CL_CMD_REG: // 注册命令处理
						if (1 != countChar(buf, ','))
						{
							puts("invalid protocal!");
							break;
						}

						GetName(buf, szName);

						// 判断名字是否重复
						if (IsExist(szName))
						{
							sprintf(repBuf, "r,exist");
						}
						else
						{
							insert(szName);
							showTable();
							sprintf(repBuf, "r,ok");
							printf("reg ok,%s\n", szName);
						}
						write(sockfd, repBuf, strlen(repBuf)); // 回复客户端
						break;

					case CL_CMD_LOGIN: // 登录命令处理
						if (1 != countChar(buf, ','))
						{
							puts("invalid protocal!");
							break;
						}

						GetName(buf, szName);

						// 判断是否注册过，即是否存在
						if (IsExist(szName))
						{
							sprintf(repBuf, "l,ok");
							printf("login ok,%s\n", szName);
						}
						else
							sprintf(repBuf, "l,noexist");
						write(sockfd, repBuf, strlen(repBuf)); // 回复客户端
						break;

					case CL_CMD_CHAT: // 聊天命令处理
						puts("send all");
						for (int i = 0; i <= maxi; i++) // 群发
							if (client[i] != -1)
								write(client[i], buf + 2, n); // 写回客户端，+2表示去掉命令头(c,)，这样只发送聊天内容
						break;
					}
				}

				if (--nready == 0)
					break;
			}
		}
	}

	close(listenfd);
	return 0;
}

// gcc *.c -o myChatSrv -lpthread -I/usr/include/mysql/ -L/usr/lib/x86_64-linux-gnu -lmysqlclient & ./myChatSrv
