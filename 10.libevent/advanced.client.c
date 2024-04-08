#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/util.h>

/*
int tcp_connect_server(const char *server_ip, int port)
{
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    int status = inet_aton(server_ip, &server_addr.sin_addr);
    if (status == 0) // the server_ip is not valid value
    {
        errno = EINVAL;
        return -1;
    }
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        return sockfd;
    status = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    int save_errno;
    if (status == -1)
    {
        save_errno = errno;
        close(sockfd);
        errno = save_errno; // the close may be error
        return -1;
    }
    evutil_make_socket_nonblocking(sockfd);
    return sockfd;
}
*/

void cmd_msg_cb(int fd, short events, void *arg)
{
    char msg[1024];
    int ret = read(fd, msg, sizeof(msg));
    if (ret < 0)
    {
        perror("read fail ");
        exit(1);
    }
    struct bufferevent *bev = (struct bufferevent *)arg;
    // 把终端的消息发送给服务器端
    bufferevent_write(bev, msg, ret);
}

void server_msg_cb(struct bufferevent *bev, void *arg)
{
    char msg[1024];
    size_t len = bufferevent_read(bev, msg, sizeof(msg));
    msg[len] = '\0';
    printf("recv %s from server\n", msg);
}

void event_cb(struct bufferevent *bev, short event, void *arg)
{
    if (event & BEV_EVENT_EOF)
        printf("connection closed\n");
    else if (event & BEV_EVENT_ERROR)
        printf("some other error\n");
    else if (event & BEV_EVENT_CONNECTED)
    {
        printf("the client has connected to server\n");
        return;
    }
    // 这将自动close套接字和free读写缓冲区
    bufferevent_free(bev);
    struct event *ev = (struct event *)arg;
    event_free(ev);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        // 两个参数依次是服务器端的IP地址、端口号
        printf("please input 2 parameter\n");
        return -1;
    }
    struct event_base *base = event_base_new();
    struct bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    // 监听终端输入事件
    struct event *ev_cmd = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, cmd_msg_cb, (void *)bev);
    event_add(ev_cmd, NULL);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &server_addr.sin_addr);
    bufferevent_socket_connect(bev, (struct sockaddr *)&server_addr, sizeof(server_addr));
    bufferevent_setcb(bev, server_msg_cb, NULL, event_cb, (void *)ev_cmd);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
    event_base_dispatch(base);
    printf("finished \n");
    return 0;
}
