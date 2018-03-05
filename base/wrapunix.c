#include <stdio.h>
#include "wrapunix.h"
#include "error.h"

void Close(int fd)
{
	if (close(fd) == -1)
	{
		err_sys("close error");
	}	
}

pid_t Fork(void)
{
	pid_t	pid;

	if ( (pid = fork()) == -1)
    {
        err_sys("fork error");
    }
		
	return(pid);
}

Sigfunc * Signal(int signo, Sigfunc *func)
{
    Sigfunc	*sigfunc;

	if ( (sigfunc = signal(signo, func)) == SIG_ERR)
    {
        err_sys("signal error");
    }
		
	return(sigfunc);
}

Sigfunc * Signal_act(int signo, Sigfunc * func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if(signo == SIGALRM)
	{
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	}
	else
	{
#ifdef SA_SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}

	if(sigaction(signo, &act, &oact) < 0)
	{
		return SIG_ERR;
	}

	return oact.sa_handler;
}

void Sigemptyset(sigset_t *set)
{
	if (sigemptyset(set) == -1)
	{
		err_sys("sigemptyset error");
	}
}

void Sigaddset(sigset_t *set, int signo)
{
	if (sigaddset(set, signo) == -1)
	{
		err_sys("sigaddset error");
	}
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout)
{
	int	n;

	if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
	{
		err_sys("select error");
	}

	return n;		/* can return 0 on timeout */
}

void Sigdelset(sigset_t *set, int signo)
{
	if (sigdelset(set, signo) == -1)
	{
		err_sys("sigdelset error");
	}
}

void Sigfillset(sigset_t *set)
{
	if (sigfillset(set) == -1)
	{
		err_sys("sigfillset error");
	}
}

int Sigismember(const sigset_t *set, int signo)
{
	int	n;

	if ( (n = sigismember(set, signo)) == -1)
	{
		err_sys("sigismember error");
	}
		
	return n;
}

void Sigpending(sigset_t *set)
{
	if (sigpending(set) == -1)
	{
		err_sys("sigpending error");
	}
}

void Sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
	if (sigprocmask(how, set, oset) == -1)
	{
		err_sys("sigprocmask error");
	}
}

int	Poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
{
	int	n;

	if ( (n = poll(fdarray, nfds, timeout)) < 0)
	{
		err_sys("poll error");
	}
		
	return n;
}

int Fcntl(int fd, int cmd, int arg)
{
	int	n;

	if ( (n = fcntl(fd, cmd, arg)) == -1)
	{
		err_sys("fcntl error");
	}
		
	return(n);
}

void * Malloc(size_t size)
{
	void *ptr;

	if ((ptr = malloc(size)) == NULL)
	{
		err_sys("malloc error");
	}
		
	return ptr;
}

void * Calloc(size_t n, size_t size)
{
	void *ptr;

	if ((ptr = calloc(n, size)) == NULL)
	{
		err_sys("calloc error");
	}
		
	return ptr;
}

int Ioctl(int fd, int request, void *arg)
{
	int n;

	if ((n = ioctl(fd, request, arg)) == -1)
	{
		err_sys("ioctl error");
	}
		
	return n;	/* streamio of I_LIST returns value */
}

void Pipe(int *fds)
{
	if (pipe(fds) < 0)
	{
		err_sys("pipe error");
	}
		
}