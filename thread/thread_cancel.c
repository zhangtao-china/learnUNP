#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define MY_USE_ASYN_CANCEL

static void errExit(int err, char *info)
{
    printf("%s error: %s\n", info, strerror(err));
    exit(-1);
}

static void * thread_func(void * arg)
{
    int j;

    printf("New thread started\n");

#ifdef MY_USE_ASYN_CANCEL
    int oldtype = 0, s;
    char msg[1024];
    s = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
    if(s != 0)
    {
        errExit(s, "pthread_setcanceltype");
    }

    if(PTHREAD_CANCEL_ASYNCHRONOUS == oldtype)
    {
        strcpy(msg, "async");
    }
    else if(PTHREAD_CANCEL_DEFERRED == oldtype)  // lip
    {
        strcpy(msg, "defered");
    }
    else
    {
        strcpy(msg, "unkown");
    }

    printf("old cancel type is %s\n", msg);
#endif
    
    for(j = 1; ; j++)
    {
#ifndef MY_USE_ASYN_CANCEL
        printf("loop %d\n", j);
        sleep(1);
#endif
    }

    printf("thread: before return\n");
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t thr;
    int s;
    void *res;

    s = pthread_create(&thr, NULL, thread_func, NULL);
    if(s != 0)
    {
        errExit(s, "pthread_create");
    }

    sleep(3);

    s = pthread_cancel(thr);
    if(s != 0)
    {
        errExit(s, "pthread_cancel");
    }

    s = pthread_join(thr, &res);
    if(s != 0)
    {
        errExit(s, "pthread_join");
    }

    if(res == PTHREAD_CANCELED)
    {
        printf("thread was canceled\n");
    }
    else
    {
        printf("thread was not canceled (should not happen)\n");
    }

    return EXIT_SUCCESS;
}