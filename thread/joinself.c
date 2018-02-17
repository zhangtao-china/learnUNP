#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

static void * thread_func(void *arg)
{
    char *s = (char *)arg;

    printf("%s", s);
    int res;

    printf("before join self.\n");
    res = pthread_join(pthread_self(), NULL);

    if(res != 0)
    {
        printf("join self error: %s\n", strerror(res));
    }
    else
    {
        printf("join success.\n");
    }

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t t;
    int res;

    res = pthread_create(&t, NULL, thread_func, "hello threads\n");

    if(res != 0)
    {
        printf("create thread error: %s\n", strerror(res));
    }

    pthread_exit(NULL);

    return 0;
}