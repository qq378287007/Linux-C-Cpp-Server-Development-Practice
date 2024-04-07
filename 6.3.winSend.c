#include <stdio.h>
#include <winsock2.h>

// #define _WINSOCK_DEPRECATED_NO_WARNINGS
// #pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);		   // 制作Winsock库的版本号
	int err = WSAStartup(wVersionRequested, &wsaData); // 初始化Winsock库
	if (err != 0)
		return 1;

	int size = sizeof(struct sockaddr_in);
	struct sockaddr_in saddr;
	memset(&saddr, 0, size);

	// 设置地址信息，ip信息
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(9999);
	saddr.sin_addr.s_addr = inet_addr("192.168.35.128"); // 该ip为服务端所在的ip

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // 创建udp 的套接字
	if (sockfd < 0)
	{
		perror("failed socket");
		return -1;
	}

	// 设置端口复用
	char on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	// 发送信息给服务端
	puts("please enter data:");
	char wbuf[50];
	scanf_s("%s", wbuf, sizeof(wbuf));
	int ret = sendto(sockfd, wbuf, sizeof(wbuf), 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
	if (ret < 0)
		perror("sendto failed");
	closesocket(sockfd);

	WSACleanup(); // 释放套接字库
	return 0;
}
