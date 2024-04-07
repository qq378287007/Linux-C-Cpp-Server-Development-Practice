#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <event2/event.h>

struct event *connev[10];

void readcb(evutil_socket_t fd, short events, void *arg)
{
	int num = *(int *)arg;
	char buf[1024];
	bzero(buf, sizeof(buf));
	int n = read(fd, buf, sizeof(buf));
	if (n <= 0)
	{
		close(fd);
		event_del(connev[num]);
		return;
	}
	send(fd, buf, n, 0);
}

void func(evutil_socket_t fd, short events, void *arg)
{
	static int num = 0;
	struct event_base *base = (struct event_base *)arg;
	struct sockaddr_in addr;
	int len = sizeof(addr);

	// 阻塞接受
	int new = accept(fd, (struct sockaddr *)&addr, &len);
	if (new > 0)
	{
		connev[num] = event_new(base, new, EV_READ | EV_PERSIST, readcb, (void *)&num);
		if (connev == NULL)
			exit(0);

		event_add(connev[num], NULL);
		num++;
	}
}

int main()
{
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0)
	{
		perror("socket");
		exit(-1);
	}

	int opt = 1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// ip配置
	struct sockaddr_in ip;
	ip.sin_family = AF_INET;
	ip.sin_addr.s_addr = htonl(INADDR_ANY);
	ip.sin_port = htons(10088);
	socklen_t len = sizeof(ip);
	if (bind(sfd, (struct sockaddr *)&ip, len))
	{
		perror("bind");
		exit(-1);
	}
	listen(sfd, 5);

	// 获取地基
	struct event_base *base = event_base_new();
	if (base == NULL)
	{
		printf("error base\n");
		exit(-1);
	}

	struct event *ev = event_new(base, sfd, EV_READ | EV_PERSIST, func, base);
	if (ev == NULL)
	{
		printf("event\n");
		exit(-1);
	}

	// 将事件添加到地基上，并调用内部的回调函数
	event_add(ev, NULL);

	// 进入循环
	event_base_dispatch(base);

	// free
	event_base_free(base);
	event_free(ev);

	close(sfd);

	return 0;
}
