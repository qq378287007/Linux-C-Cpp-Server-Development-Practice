#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

int main()
{
	int sfp = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sfp)
	{
		printf("socket() fail!\n");
		return -1;
	}
	printf("socket() ok!\n");

	char on = 1;
	setsockopt(sfp, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	struct sockaddr_in serv = {0};
	int serv_len = sizeof(serv);
	printf("ip=%s, port=%d\n", inet_ntoa(serv.sin_addr), ntohs(serv.sin_port));

	getsockname(sfp, (struct sockaddr *)&serv, (socklen_t *)&serv_len);
	printf("ip=%s, port=%d\n", inet_ntoa(serv.sin_addr), ntohs(serv.sin_port));

	unsigned short portnum = 10051;
	struct sockaddr_in s_add;
	memset(&s_add, 0, sizeof(struct sockaddr_in));
	s_add.sin_family = AF_INET;
	s_add.sin_addr.s_addr = inet_addr("127.0.0.1");
	s_add.sin_port = htons(portnum);

	if (-1 == bind(sfp, (struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
		printf("bind() fail: %d!\n", errno);
		return -1;
	}
	printf("bind() ok!\n");

	getsockname(sfp, (struct sockaddr *)&serv, (socklen_t *)&serv_len);
	printf("ip=%s, port=%d\n", inet_ntoa(serv.sin_addr), ntohs(serv.sin_port));

	return 0;
}
