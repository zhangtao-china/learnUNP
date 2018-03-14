#include "../base/wrapunix.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "heartbeat.h"

static int servfd;
static int maxnalarms;
static int nsec;
static int nprobes;
static void sig_urg(int), sig_alrm(int);

void heartbeat_serv(int servfd_arg, int nsec_arg, int maxnalarms_arg)
{
    servfd = servfd_arg;
    if((nsec = nsec_arg) < 1)
    {
        nsec = 1;
    }
    if((maxnalarms = maxnalarms_arg) < nsec)
    {
        maxnalarms = nsec;
    }

    Signal(SIGURG, sig_urg);
    Fcntl(servfd, F_SETOWN, getpid());

    Signal(SIGALRM, sig_alrm);
    alarm(nsec);
}

void sig_urg(int signo)
{
    int n;
    char c;

    if((n = recv(servfd, &c, 1, MSG_OOB)) < 0)
    {
        if(errno != EWOULDBLOCK)
        {
            err_sys("recv error");
        }
    }

    Send(servfd, &c, 1, MSG_OOB);

    nprobes = 0;

    return;
}

void sig_alrm(int signo)
{
    if(++nprobes > maxnalarms)
    {
        printf("no probes from client\n");
        exit(0);
    }

    alarm(nsec);

    return;
}