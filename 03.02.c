#include <pthread.h>
#include <stdio.h>

void *thfunc(void *arg)
{
    int n = *(int *)(arg);
    printf("in thfunc: n = %d\n", n);
    return (void *)0;
}

int main()
{
    int n = 110;

    pthread_t tidp;
    int ret = pthread_create(&tidp, NULL, thfunc, &n);
    if (ret)
    {
        printf("pthread_create failed: %d\n", ret);
        return -1;
    }
    pthread_join(tidp, NULL);

    printf("in main: thread is created\n");

    return 0;
}
