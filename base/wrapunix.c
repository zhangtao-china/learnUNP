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

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout)
{
	int	n;

	if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
	{
		err_sys("select error");
	}

	return(n);		/* can return 0 on timeout */
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