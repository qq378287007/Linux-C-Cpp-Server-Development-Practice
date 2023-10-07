#include <stdio.h>
#include <winsock.h>

//#pragma comment(lib, "wsock32")

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
	saddr.sin_port = htons(PORT);
	saddr.sin_addr.s_addr = inet_addr("192.168.81.128");

	/**** get protocol number  from protocol name  ****/
	// struct hostent *phe; // host information
	// struct servent *pse; // server information
	struct protoent *ppe; // protocol information
	if ((ppe = getprotobyname("UDP")) == 0)
	{
		printf("get protocol information error \n");
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

	int fromlen = sizeof(struct sockaddr);
	printf("please enter data:");

	char wbuf[50];
	scanf_s("%s", wbuf, sizeof(wbuf));

	int ret = sendto(s, wbuf, sizeof(wbuf), 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
	if (ret < 0)
		perror("sendto failed");

	char rbuf[100];
	struct sockaddr_in raddr; /*endpoint IP address  */
	int len = recvfrom(s, rbuf, sizeof(rbuf), 0, (struct sockaddr *)&raddr, &fromlen);
	if (len < 0)
		perror("recvfrom failed");
	printf("server reply:%s\n", rbuf);

	closesocket(s);

	WSACleanup();

	return 0;
}
