#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int glob = 0;

static void errExit(int err, char *info)
{
    printf("%s error: %s\n", info, strerror(err));
    exit(-1);
}

static void cleanup_func(void * arg)
{
    int s;

    printf("cleanup: freeing block at %p\n", arg);
    free(arg);

    printf("cleanup: unlocking mutex\n");

    s = pthread_mutex_unlock(&mtx);
    if(s != 0)
    {
        errExit(s, "pthread_mutex_unlock");
    }
}

static void * thread_func(void * arg)
{
    int s;

    char * buf = NULL;

    buf = malloc(0x10000);
    if(buf == NULL)
    {
        errExit(errno, "malloc");
    }
    else
    {
        printf("thread: allocated memory at %p\n", buf);
    }
    
    s = pthread_mutex_lock(&mtx);
    if(s != 0)
    {
        errExit(s, "pthread_mutex_lock");
    }

    pthread_cleanup_push(cleanup_func, buf);

    while(glob == 0)
    {
        s = pthread_cond_wait(&cond, &mtx);
        if(s != 0)
        {
            errExit(s, "pthread_cond_wait");
        }
    }

    printf("thread: condition wait loop completed\n");
    pthread_cleanup_pop(1);

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

    sleep(2);

    if(argc == 1)
    {
        printf("main: about to cancel thread\n");
        s = pthread_cancel(thr);
        if(s != 0)
        {
            errExit(s, "pthread_cancel");
        }
    }
    else
    {
        printf("main: about to signal condition variable\n");
        glob = 1;
        s = pthread_cond_signal(&cond);
        if(s != 0)
        {
            errExit(s, "pthread_cond_signal");
        }
    }

    s = pthread_join(thr, &res);
    if(s != 0)
    {
        errExit(s, "pthread_join");
    }

    if(res == PTHREAD_CANCELED)
    {
        printf("main: thread was canceled\n");
    }
    else
    {
        printf("main: thread terminated normally\n");
    }

    return EXIT_SUCCESS;
}