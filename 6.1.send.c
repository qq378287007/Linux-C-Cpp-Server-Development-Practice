#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main()
{
	int size = sizeof(struct sockaddr_in);
	struct sockaddr_in saddr;
	memset(&saddr, 0, size);

	// 设置服务端的地址信息
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr("192.168.0.118"); // 该ip为服务端所在的ip
	saddr.sin_port = htons(9999);

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // 创建udp 的套接字
	if (sockfd < 0)
	{
		perror("failed socket");
		return -1;
	}

	// 设置端口复用，就是释放后，能马上再次使用
	char on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	// 发送信息给服务端
	puts("please enter data:");
	char wbuf[50];
	scanf("%s", wbuf, sizeof(wbuf));
	int ret = sendto(sockfd, wbuf, sizeof(wbuf), 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
	if (ret < 0)
		perror("sendto failed");
	close(sockfd);
	return 0;
}
