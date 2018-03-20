#include <signal.h>
#include "wrappthread.h"
#include "error.h"


void Pthread_create(pthread_t *tid, const pthread_attr_t *attr, void * (*func)(void *), void *args)
{
    int n;
    if((n = pthread_create(tid, attr, func, args)) == 0)
    {
        return;
    }

    errno = n;

    err_sys("pthread_create error");
}

void Pthread_join(pthread_t tid, void **status)
{
    int n;
    if((n = pthread_join(tid, status)) == 0)
    {
        return;
    }

    errno = n;

    err_sys("pthread_join error");
}

void Pthread_detach(pthread_t tid)
{
    int n;
    if((n = pthread_detach(tid)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_detach error");
}

void Pthread_kill(pthread_t tid, int signo)
{
    int n;
    if((n = pthread_kill(tid, signo)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_kill error");
}

void Pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    int n;
    if((n = pthread_mutexattr_init(attr)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_mutexattr_init error");
}

#ifdef	_POSIX_THREAD_PROCESS_SHARED
void Pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int flag)
{
    int n;
    if((n = pthread_mutexattr_setpshared(attr, flag)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_mutexattr_setpshared error");
}
#endif

void Pthread_mutex_init(pthread_mutex_t *mptr, pthread_mutexattr_t *attr)
{
    int n;
    if((n = pthread_mutex_init(mptr, attr)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_mutex_init error");
}

void Pthread_mutex_lock(pthread_mutex_t *mptr)
{
    int n;
    if((n = pthread_mutex_lock(mptr)) == 0)
    {
        return;
    }

    errno = 0;
    err_sys("pthread_mutex_lock error");
}

void Pthread_mutex_unlock(pthread_mutex_t *mptr)
{
    int n;
    if((n = pthread_mutex_unlock(mptr)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_mutex_unlock error");
}

void Pthread_cond_broadcast(pthread_cond_t *cptr)
{
    int n;
    if((n = pthread_cond_broadcast(cptr)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_cond_broadcast error");
}

void Pthread_cond_signal(pthread_cond_t *cptr)
{
    int n;
    if((n = pthread_cond_signal(cptr)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_cond_signal error");
}

void Pthread_cond_wait(pthread_cond_t *cptr, pthread_mutex_t *mptr)
{
    int n;
    if((n = pthread_cond_wait(cptr, mptr)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_cond_wait error");
}

int Pthread_cond_timedwait(pthread_cond_t *cptr, pthread_mutex_t *mptr, const struct timespec *tsptr)
{
    int n;
    if((n = pthread_cond_timedwait(cptr, mptr, tsptr)) == 0)
    {
        return n;
    }
    else if(n != ETIMEDOUT && n != EINTR)
    {
        errno = n;
        err_sys("pthread_cond_timedwait error");
    }

    return n;
}

void Pthread_key_create(pthread_key_t *key, void (*func)(void *))
{
    int n;
    if((n = pthread_key_create(key, func)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_key_create error");
}

void Pthread_setspecific(pthread_key_t key, const void *value)
{
    int n;
    if((n = pthread_setspecific(key, value)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_setspecific error");
}

void Pthread_once(pthread_once_t *ptr, void (*func)(void))
{
    int n;
    if((n = pthread_once(ptr, func)) == 0)
    {
        return;
    }

    errno = n;
    err_sys("pthread_once error");
}