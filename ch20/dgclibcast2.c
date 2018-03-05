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
        err_quit("usage: dgclibcast2 <IPaddress>");
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
    socklen_t len;
    struct sockaddr *preply_addr;
    sigset_t sigset_alarm;

    preply_addr = Malloc(servlen);

    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    Sigemptyset(&sigset_alarm);
    Sigaddset(&sigset_alarm, SIGALRM);
    Signal(SIGALRM, recvfrom_alarm);

/*
   错误示范
*/
    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        alarm(5);  // start timer
        for(;;)
        {
            len = servlen;
            /*
            缺陷：信号可能恰在recvfrom返回最后一个应答数据报之后与sigprocmask调用之间递交，那么下一次调用recvfrom将永远阻塞
            */
            Sigprocmask(SIG_UNBLOCK, &sigset_alarm, NULL);
            n = recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
            Sigprocmask(SIG_BLOCK, &sigset_alarm, NULL);
            if(n < 0)
            {
                if(errno == EINTR)
                {
                    break;
                }
                else
                {
                    err_sys("recvfrom error");
                }
            }
            else 
            {
                recvline[n] = 0;
                printf("from %s: %s", sock_ntop_host(preply_addr, len), recvline);
            }
        }
    }

    free(preply_addr);
}


static void recvfrom_alarm(int signo)
{
    return ; 
}