
#include <event2/event.h>

int main()
{
    printf("version: %s\n", event_get_version());

    const char **methods = event_get_supported_methods();
    for (int i = 0; methods[i] != NULL; i++)
        printf("method %d: %s\n", i, methods[i]);

    struct event_base *eb = event_base_new(); // error返回NULL
    event_base_free(eb);

    return 0;
}

// typedef void (*event_callback_fn)(evutil_socket_t fd, short events, void * arg);
// struct event *event_new(struct event_base *base, evutil_socket_t fd, short events, event_callback_fn cb, void *arg);
// void event_free(struct event *event);
// #define EV_TIMEOUT      0x01
// #define EV_READ         0x02
// #define EV_WRITE        0x04
// #define EV_SIGNAL       0x08 //用于实现信号检测
// #define EV_PERSIST      0x10 //表示事件是“持久的”
// #define EV_ET           0x20 //边沿触发，一次

// int event_add(struct event *ev, const struct timeval *tv);
// int event_del(struct event *ev);

// #define EVLOOP_ONCE 0x01
// #define EVLOOP_NONBLOCK         0x02
// int event_base_loop(struct event_base *base, int flags);
// int event_base_dispatch(struct event_base *base);

// int event_base_loopexit(struct event_base *base, const struct timeval *tv);
// int event_base_loopbreak(struct event_base *base);

// struct event_config *event_config_new(void);
// struct event_base *event_base_new_with_config(const struct event_config *cfg);
// void event_config_free(struct event_config *cfg);
// int event_config_avoid_method(struct event_config *cfg, const char *method);

// int event_config_require_features(struct event_config *cfg, enum event_method_feature feature);
// enum event_method_feature {
//   EV_FEATURE_ET = 0x01, //要求支持边沿触发的后端
//   EV_FEATURE_O1 = 0x02, //要求添加、删除单个事件，或者确定哪个事件激活的操作是O（1）复杂度的后端
//   EV_FEATURE_FDS = 0x04, //要求支持任意文件描述符，而不仅仅是套接字的后端
// };

// int event_config_set_flag(struct event_config *cfg, enum event_base_config_flag flag);
// enum event_base_config_flag {
//   EVENT_BASE_FLAG_NOLOCK = 0x01,
//   EVENT_BASE_FLAG_IGNORE_ENV = 0x02,
//   EVENT_BASE_FLAG_STARTUP_IOCP = 0x04,
//   EVENT_BASE_FLAG_NO_CACHE_TIME = 0x08,
//   EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST = 0x10,
//   EVENT_BASE_FLAG_PRECISE_TIMER = 0x20
// };

// int event_base_priority_init(struct event_base *base, int n_priorities);
