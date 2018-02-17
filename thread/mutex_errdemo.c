#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static int glob = 0;

static void * thread_func(void *arg)
{
    int loops = *(int*)arg;
    int loc, j;

    for(j = 0; j < loops; j++)
    {
#if 0
        loc = glob;
        loc++;
        glob = loc;
#else
        glob++;
#endif
    }

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t t1, t2;
    int loops = 0, s;

    if(argc > 1)
    {
        loops = atoi(argv[1]);
    }

    if(0 == loops)
    {
        loops = 10000000;
    }

    s = pthread_create(&t1, NULL, thread_func, &loops);
    if(s != 0)
    {
        printf("create thread 1 error\n");
        return -1;
    }

    s = pthread_create(&t2, NULL, thread_func, &loops);
    if(s != 0)
    {
        printf("create thread 2 error\n");
        return -1;
    }

    s = pthread_join(t1, NULL);
    if(s != 0)
    {
        printf("join thread 1 error\n");
        return -1;
    }

    s = pthread_join(t2, NULL);
    if(s != 0)
    {
        printf("join thread 2 error\n");
        return -1;
    }

    printf("glob = %d\n", glob);

    return EXIT_SUCCESS;
}