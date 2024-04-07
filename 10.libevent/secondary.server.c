#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <event.h>
#include <event2/bufferevent.h>

int tcp_server_init(int port, int listen_num)
{
    int errno_save;
    evutil_socket_t listener = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1)
        return -1;

    evutil_make_listen_socket_reuseable(listener); // 多次绑定同一地址
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(port);
    if (::bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        goto error;
    if (::listen(listener, listen_num) < 0)
        goto error;

    evutil_make_socket_nonblocking(listener); // 非阻塞状态
    return listener;
error:
    errno_save = errno;
    evutil_closesocket(listener);
    errno = errno_save;
    return -1;
}

void socket_read_cb(bufferevent *bev, void *arg)
{
    char msg[4096];
    size_t len = bufferevent_read(bev, msg, sizeof(msg));
    msg[len] = '\0';
    printf("recv the client msg: %s", msg);
    char reply_msg[4096] = "I have recvieced the msg: ";
    strcat(reply_msg + strlen(reply_msg), msg);
    bufferevent_write(bev, reply_msg, strlen(reply_msg));
}

void event_cb(struct bufferevent *bev, short event, void *arg)
{
    if (event & BEV_EVENT_EOF)
        printf("connection closed\n");
    else if (event & BEV_EVENT_ERROR)
        printf("some other error\n");
    // 这将自动close套接字和free读写缓冲区
    bufferevent_free(bev);
}

void accept_cb(int fd, short events, void *arg)
{
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    evutil_socket_t sockfd = ::accept(fd, (struct sockaddr *)&client, &len);
    evutil_make_socket_nonblocking(sockfd);
    printf("accept a client %d\n", sockfd);
    struct event_base *base = (event_base *)arg;
    bufferevent *bev = bufferevent_socket_new(base, sockfd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, socket_read_cb, NULL, event_cb, arg);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

int main()
{
    int listener = tcp_server_init(9999, 10);
    if (listener == -1)
    {
        perror(" tcp_server_init error ");
        return -1;
    }
    struct event_base *base = event_base_new();
    // 添加监听客户端请求连接事件
    struct event *ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, accept_cb, base);
    event_add(ev_listen, NULL);
    event_base_dispatch(base);
    event_base_free(base);
    return 0;
}
