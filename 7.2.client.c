﻿
#include <stdio.h>
#include <winsock.h>

// #pragma comment(lib, "wsock32")

#define PORT 8888

int main()
{
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 0), &wsadata) != 0)
	{
		printf("WSAStartup failed\n");
		WSACleanup();
		return -1;
	}

	struct sockaddr_in saddr; // endpoint IP address
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);
	saddr.sin_addr.s_addr = inet_addr("192.168.81.128");

	SOCKET s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		printf(" creat socket error \n");
		WSACleanup();
		return -1;
	}
	if (connect(s, (struct sockaddr *)&saddr, sizeof(saddr)) == SOCKET_ERROR)
	{
		printf("connect socket error\n");
		WSACleanup();
		return -1;
	}

	printf("please enter data:");
	char wbuf[50];
	scanf_s("%s", wbuf, sizeof(wbuf));
	int len = send(s, wbuf, sizeof(wbuf), 0);
	if (len < 0)
		perror("send failed");

	char rbuf[100];
	len = recv(s, rbuf, sizeof(rbuf), 0);
	if (len < 0)
		perror("recv failed");
	printf("server reply:%s\n", rbuf);

	closesocket(s);

	WSACleanup();

	return 0;
}
