#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#define PORT 8888

int main()
{
	struct sockaddr_in sin; // endpoint IP address
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(PORT);

	int s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		printf("creat socket error\n");
		return -1;
	}

	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		printf("socket bind error\n");
		return -1;
	}

	if (listen(s, 10) == -1)
	{
		printf("  socket listen error\n");
		return -1;
	}

	int alen = sizeof(struct sockaddr);
	struct sockaddr_in fsin;
	int connum = 0;
	while (1)
	{
		puts("waiting client...");
		int clisock = accept(s, (struct sockaddr *)&fsin, (socklen_t *)&alen);
		if (clisock == -1)
		{
			printf("accept failed\n");
			return -1;
		}

		connum++;
		printf("%d client comes\n", connum);

		char rbuf[64] = {0};
		int len = recv(clisock, rbuf, sizeof(rbuf), 0);
		if (len < 0)
			perror("recv failed");

		char buf[128] = {0};
		sprintf(buf, "Server has received your data(%s).", rbuf);
		send(clisock, buf, strlen(buf), 0);
		close(clisock);
	}

	close(s);

	return 0;
}
