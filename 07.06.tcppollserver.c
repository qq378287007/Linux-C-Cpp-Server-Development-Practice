
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

void errExit()
{
	exit(-1);
}

const char resp[] = "HTTP/1.1 200\r\n\
Content-Type: application/json\r\n\
Content-Length: 13\r\n\
Date: Thu, 2 Aug 2021 04:02:00 GMT\r\n\
Keep-Alive: timeout=60\r\n\
Connection: keep-alive\r\n\
\r\n\
[HELLO WORLD]";

int main()
{
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
		errExit();
	fprintf(stderr, "created socket\n");

	int opt = 1;
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		errExit();
	fprintf(stderr, "socket opt set\n");

	const int port = 8888;
	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	socklen_t addrLen = sizeof(addr);
	if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		errExit();
	fprintf(stderr, "socket binded\n");

	if (listen(sd, 1024) == -1)
		errExit();
	fprintf(stderr, "socket listen start\n");

	//  number of poll fds
	int currentFdNum = 1;
	struct pollfd *fds = (struct pollfd *)calloc(100, sizeof(struct pollfd));
	fds[0].fd = sd;
	fds[0].events = POLLIN;
	nfds_t nfds = 1;

	fprintf(stderr, "polling\n");
	while (1)
	{
		int timeout = -1;
		int ret = poll(fds, nfds, timeout);
		fprintf(stderr, "poll returned with ret value: %d\n", ret);
		if (ret == -1)
			errExit();
		else if (ret == 0)
			fprintf(stderr, "return no data\n");
		else
		{
			fprintf(stderr, "checking fds\n");
			if (fds[0].revents & POLLIN)
			{
				struct sockaddr_in childAddr;
				socklen_t childAddrLen;
				int childSd = accept(sd, (struct sockaddr *)&childAddr, &(childAddrLen));
				if (childSd == -1)
					errExit();
				fprintf(stderr, "child got\n");

				// set non_block
				int flags = fcntl(childSd, F_GETFL);
				if (fcntl(childSd, F_SETFL, flags | O_NONBLOCK) == -1)
					errExit();
				fprintf(stderr, "child set nonblock\n");

				// add child to list
				fds[currentFdNum].fd = childSd;
				fds[currentFdNum].events = (POLLIN | POLLRDHUP);
				nfds++;
				currentFdNum++;
				fprintf(stderr, "child: %d pushed to poll list\n", currentFdNum - 1);
			}

			// child read & write
			for (int i = 1; i < currentFdNum; i++)
			{
				if (fds[i].revents & (POLLHUP | POLLRDHUP | POLLNVAL))
				{
					fprintf(stderr, "child: %d shutdown\n", i);
					close(fds[i].fd);
					fds[i].events = 0;
					fds[i].fd = -1;
					continue;
				}

				//  read
				if (fds[i].revents & POLLIN)
				{
					char buffer[1024] = {0};
					while (1)
					{
						ret = read(fds[i].fd, buffer, 1024);
						fprintf(stderr, "read on: %d returned with value: %d\n", i, ret);
						if (ret == 0)
						{
							fprintf(stderr, "read returned 0(EOF) on: %d, breaking\n", i);
							break;
						}
						else if (ret == -1)
						{
							const int tmpErrno = errno;
							if (tmpErrno == EWOULDBLOCK || tmpErrno == EAGAIN)
							{
								fprintf(stderr, "read would block, stop reading\n");
								fds[i].events |= POLLOUT;
								break;
							}
							else
							{
								errExit();
							}
						}
					}
				}

				// write
				if (fds[i].revents & POLLOUT)
				{
					ret = write(fds[i].fd, resp, sizeof(resp));
					fprintf(stderr, "write on: %d returned with value: %d\n", i, ret);
					if (ret == -1)
						errExit();
					fds[i].events &= !(POLLOUT);
				}
			}
		}
	}

	return 0;
}
