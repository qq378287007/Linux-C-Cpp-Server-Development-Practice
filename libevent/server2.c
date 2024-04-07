#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include <event2/event.h>

#define SERVER_PORT 8080

void ReadCB(evutil_socket_t fd, short what, void *arg)
{
    printf("READCB func start\n");
    char buffer[1024] = "welcome to xms";
    int length = send(fd, buffer, strlen(buffer), 0);
    printf("length = %d\n", length);

    length = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (length > 0)
    {
        buffer[length] = '\0';
        printf("%s\n", buffer);
    }
    printf("READCB func end\n");
    close(fd);

    // event_del(clientEv);
    // event_free(clientEv);
}

void AcceptCB(evutil_socket_t fd, short what, void *arg)
{
    struct sockaddr_in clientAddr;
    socklen_t len = 0;
    int clientFd = accept(fd, (struct sockaddr *)&clientAddr, &len);
    printf("clientFd = %d\n", clientFd);

    struct event_base *base = (struct event_base *)arg;
    struct event *clientEv = event_new(base, clientFd, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, ReadCB, NULL);
    event_add(clientEv, NULL);
}

void MainLoop(int fd)
{
    struct event_base *base = event_base_new();
    struct event *listen = event_new(base, fd, EV_READ | EV_WRITE | EV_PERSIST | EV_ET, AcceptCB, base);

    event_add(listen, NULL);
    event_base_dispatch(base);

    event_del(listen);
    event_free(listen);
    event_base_free(base);
}

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SERVER_PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(fd, (struct sockaddr *)&saddr, sizeof(saddr)) != 0)
    {
        printf("bind error\n");
        exit(1);
    }
    printf("bind port %d success\n", SERVER_PORT);

    listen(fd, 10);

    MainLoop(fd);

    close(fd);
    return 0;
}