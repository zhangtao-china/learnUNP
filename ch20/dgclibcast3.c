#include <string.h>
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/wrapunix.h"

static void recvfrom_alarm(int);
void dg_cli(FILE *fp, int sockfd, const sockaddr * pservaddr, socklen_t servlen);

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if(argc != 2)
    {
        err_quit("usage: dgclibcast3 <IPaddress>");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    dg_cli(stdin, sockfd, (sockaddr*)&servaddr, sizeof(servaddr));

    return 0;
}

// 设置SO_BROADCAST套接字选项并显示在5s内收到的所有应答
void dg_cli(FILE *fp, int sockfd, const sockaddr * pservaddr, socklen_t servlen)
{
    int n;
    const int on = 1;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    fd_set rset;
    socklen_t len;
    struct sockaddr *preply_addr;
    sigset_t sigset_alarm, sigset_empty;

    preply_addr = Malloc(servlen);

    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    Sigemptyset(&sigset_empty);
    Sigemptyset(&sigset_alarm);
    Sigaddset(&sigset_alarm, SIGALRM);

    Signal(SIGALRM, recvfrom_alarm);

/*
   正确示范1   use pselect
*/
    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        Sigprocmask(SIG_BLOCK, &sigset_alarm, NULL);
        alarm(5);  // start timer
        for(;;)
        {
            FD_SET(sockfd, &rset);
            n = pselect(sockfd+1, &rset, NULL, NULL, NULL, &sigset_empty);
            if(n < 0)
            {
                if(errno == EINTR)
                {
                    break;
                }
                else
                {
                    err_sys("pselect error");
                }
            }
            else if(n != 1)
            {
                err_sys("pselect error: returned %d", n);
            }

            len = servlen;
            n = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
            recvline[n] = 0;
            printf("from %s: %s", sock_ntop_host(preply_addr, len), recvline);

        }
    }

    free(preply_addr);
}


static void recvfrom_alarm(int signo)
{
    return ; 
}