#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define THREAD_COUNT (2)

static __thread int glob = 0;

void * thread_func(void * arg)
{
    printf("thread id %ld, &glob = %p\n", pthread_self(), &glob);

    return NULL;
}

int main(int argc, char **argv)
{
    int s, i;
    pthread_t thr[THREAD_COUNT];

    for(i = 0; i < THREAD_COUNT; i++)
    {
        s = pthread_create(&thr[i], NULL, thread_func, NULL);
        if(s != 0)
        {
            printf("pthread_create error: %s\n", strerror(s));
            return -1;
        }
    }

    printf("main, &glob = %p\n", &glob);
    
    for(i = 0; i < THREAD_COUNT; i++)
    {
        s = pthread_join(thr[i], NULL);
        if(s != 0)
        {
            printf("pthread_join error: %s\n", strerror(s));
            return -1;
        }
    }

    return 0;
}