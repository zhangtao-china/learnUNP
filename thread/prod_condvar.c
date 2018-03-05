#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int avail = 0;

typedef enum { FALSE, TRUE } Boolean;

static void errExit(int err, char *info)
{
    printf("%s error: %s\n", info, strerror(err));
    exit(-1);
}

static void *thread_func(void *arg)
{
    int cnt = atoi((char *) arg);
    int s, j;

    for (j = 0; j < cnt; j++) {
        sleep(1);

        /* Code to produce a unit omitted */

        s = pthread_mutex_lock(&mtx);
        if (s != 0)
        {
            errExit(s, "pthread_mutex_lock");
        }

        printf("[thread]***************%ld****************get mutex lock\n", pthread_self());

        avail++;        /* Let consumer know another unit is available */

        s = pthread_mutex_unlock(&mtx);
        if (s != 0)
        {
            errExit(s, "pthread_mutex_unlock");
        }

        printf("[thread]***************%ld****************unlock mutex\n", pthread_self());

        s = pthread_cond_signal(&cond);
        if(s != 0)
        {
            errExit(s, "pthread_cond_signal");
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    int s, j;
    int totRequired;            /* Total number of units that all
                                   threads will produce */
    int numConsumed;            /* Total units so far consumed */
    Boolean done;
    time_t t;

    t = time(NULL);

    /* Create all threads */

    totRequired = 0;
    for (j = 1; j < argc; j++) 
    {
        totRequired += atoi(argv[j]);

        s = pthread_create(&tid, NULL, thread_func, argv[j]);
        if (s != 0)
        {
            errExit(s, "pthread_create");
        }
    }

    /* Use a polling loop to check for available units */

    numConsumed = 0;
    done = FALSE;

    for (;;) 
    {
        s = pthread_mutex_lock(&mtx);
        printf("[main]----------------------------get mutex lock\n");
        if (s != 0)
        {
            errExit(s, "pthread_mutex_lock");
        }
        
/*
    是while而不是if的原因
    1.其他线程可能率先醒来。也许有多个线程在等待获取与条件变量相关的互斥量。
    其他线程如果同时接收到通知有可能率先获取互斥量，并改变相关共享变量的状态，进而改变判断条件的状态
    2.设计时设置“宽松的”判断条件或许更简单。有时，用条件变量表征可能性而非确定性，在设计应用程序时更为简单
    3.可能会发生虚假唤醒情况。在一些多处理器系统上，为确保高效实现而采用的技术会导致此类虚假唤醒
*/
        while(avail == 0)
        {
/*
    pthread_cond_wait
    1. 线程在检查共享变量状态时锁定互斥量
    2. 检查共享变量的状态
    3. 如果共享变量未处于预期状态，线程应该在等待条件变量并进入休眠前解锁互斥量，以便其他线程能访问该互斥量
    4. 当线程因为条件变量的通知而再度被唤醒时，必须对互斥量再次加锁，因为在典型情况下，线程会立即访问共享变量

    ☆☆☆ pthread_cond_wait会自动做最后两步中对互斥量的解锁和加锁动作
*/
            printf("[main]----------------------------call pthread_cond_wait\n");
            s = pthread_cond_wait(&cond, &mtx);
            printf("[main]----------------------------pthread_cond_wait returned\n");
            if(s != 0)
            {
                errExit(s, "pthread_cond_wait");
            }
        }
    
        while (avail > 0)              /* Consume all available units */
        {
            /* Do something with produced unit */
            numConsumed ++;
            avail--;
            printf("T=%ld: numConsumed=%d\n", (long) (time(NULL) - t),
                    numConsumed);

            done = numConsumed >= totRequired;
        }

        s = pthread_mutex_unlock(&mtx);
        if (s != 0)
        {
            errExit(s, "pthread_mutex_unlock");
        }
        printf("[main]----------------------------unlock mutex\n");

        if (done)
            break;

        /* Perhaps do other work here that does not require mutex lock */
    }

    exit(EXIT_SUCCESS);
}
