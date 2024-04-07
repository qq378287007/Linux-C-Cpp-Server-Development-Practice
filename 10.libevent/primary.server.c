#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <event.h>

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

void socket_read_cb(int fd, short events, void *arg)
{
    struct event *ev = (struct event *)arg;
    char msg[4096];
    int len = read(fd, msg, sizeof(msg) - 1);
    if (len <= 0)
    {
        printf("some error happen when read\n");
        event_free(ev);
        close(fd);
        return;
    }
    msg[len] = '\0';
    printf("recv the client msg: %s", msg);
    char reply_msg[4096] = "I have recvieced the msg: ";
    strcat(reply_msg + strlen(reply_msg), msg);
    write(fd, reply_msg, strlen(reply_msg));
}

void accept_cb(int fd, short events, void *arg)
{
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    evutil_socket_t sockfd = ::accept(fd, (struct sockaddr *)&client, &len);
    evutil_make_socket_nonblocking(sockfd);
    printf("accept a client %d\n", sockfd);
    struct event_base *base = (struct event_base *)arg;
    // 仅仅是为了动态创建一个event结构体
    struct event *ev = event_new(NULL, -1, 0, NULL, NULL);
    // 将动态创建的结构体作为event的回调参数
    event_assign(ev, base, sockfd, EV_READ | EV_PERSIST, socket_read_cb, (void *)ev);
    event_add(ev, NULL);
}

int main()
{
    int listener = tcp_server_init(9999, 10);
    if (listener == -1)
    {
        perror("tcp_server_init error");
        return -1;
    }
    struct event_base *base = event_base_new();
    // 添加监听客户端请求连接事件
    struct event *ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, accept_cb, base);
    event_add(ev_listen, NULL);
    event_base_dispatch(base);
    return 0;
}