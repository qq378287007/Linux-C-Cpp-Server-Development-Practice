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

	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // ifconfig
	saddr.sin_port = htons(PORT);

	/**** get protocol number  from protocol name  ****/
	// struct hostent *phe; // host information
	// struct servent *pse; // server information
	struct protoent *ppe; // protocol information
	if ((ppe = getprotobyname("UDP")) == 0)
	{
		printf("get protocol information error\n");
		WSACleanup();
		return -1;
	}

	SOCKET s = socket(PF_INET, SOCK_DGRAM, ppe->p_proto);
	if (s == INVALID_SOCKET)
	{
		printf(" creat socket error \n");
		WSACleanup();
		return -1;
	}

	char wbuf[50] = "hello, server!";
	// printf("please enter data:");
	// sscanf_s("%s", wbuf, sizeof(wbuf));

	int ret = sendto(s, wbuf, strlen(wbuf), 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
	if (ret < 0)
		perror("sendto failed");

	char rbuf[100] = {0};
	struct sockaddr_in raddr; // endpoint IP address
	int fromlen = sizeof(struct sockaddr);
	int len = recvfrom(s, rbuf, sizeof(rbuf), 0, (struct sockaddr *)&raddr, &fromlen);
	if (len < 0)
		perror("recvfrom failed");
	printf("server reply: %s\n", rbuf);

	closesocket(s);

	WSACleanup();

	return 0;
}

// gcc 7.udpclient.c -o 7.udpclient.exe -lwsock32 && 7.udpclient.exe
