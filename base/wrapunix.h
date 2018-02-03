#ifndef WRAP_UNIX_H
#define WRAP_UNIX_H

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/poll.h>


void Close(int);

pid_t Fork(void);

typedef	void Sigfunc(int);

Sigfunc * Signal(int, Sigfunc *);

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout);

int	Poll(struct pollfd *, unsigned long, int);

#endif