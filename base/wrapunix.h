#ifndef WRAP_UNIX_H
#define WRAP_UNIX_H

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <sys/ioctl.h>


void Close(int);

pid_t Fork(void);

typedef	void Sigfunc(int);

Sigfunc * Signal(int, Sigfunc *);

void Sigemptyset(sigset_t *);

void Sigaddset(sigset_t *, int);

void Sigdelset(sigset_t *set, int signo);

void Sigfillset(sigset_t *);

int Sigismember(const sigset_t *, int);

void Sigpending(sigset_t *);

void Sigprocmask(int , const sigset_t *, sigset_t *);

Sigfunc * Signal_act(int, Sigfunc *);

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout);

int	Poll(struct pollfd *, unsigned long, int);

int Fcntl(int fd, int cmd, int arg);

int Ioctl(int, int, void *);

void * Malloc(size_t);

void *Calloc(size_t, size_t);

void Pipe(int *);

#endif