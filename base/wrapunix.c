#include <signal.h>
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