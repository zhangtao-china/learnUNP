#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#define MAX_LINE (1024)

// #define USE_SIGACTION

static void on_sig_alrm(int);

int main(int argc, char **argv)
{
    
    char buff[1024];

/*
  Ubuntu 16.04.3
  sigaction 安装alarm信号默认行为是中断fgets调用， 而signal函数的默认行为是被信号中断的调用会继续进行
*/

#ifdef USE_SIGACTION
    struct sigaction act, oact;
    
    act.sa_handler = on_sig_alrm;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
/*
#  ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#  endif
*/
	if(sigaction(SIGALRM, &act, &oact) < 0)
	{
        printf("sigaction error\n");
		return -1;
	}
#else
    //sighandler_t sigfunc;
    /*sigfunc =*/ signal(SIGALRM, on_sig_alrm);
#endif

    printf("please input int 5 seconds\n");
        
    alarm(5);
    while(fgets(buff, 1024, stdin) != NULL)
    {
        fputs(buff, stdout);
    }

    if(EINTR == errno)
    {
        printf("input timeout, closed\n");
    }
    else
    {
        perror("other error:");
        return -1;
    }

    return 0;
}

static void on_sig_alrm(int signo)
{
    printf("alarm timeout.\n");
    return;
}