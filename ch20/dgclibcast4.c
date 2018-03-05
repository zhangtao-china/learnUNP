#include <string.h>
#include <setjmp.h>
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

static sigjmp_buf jmpbuf;

// 设置SO_BROADCAST套接字选项并显示在5s内收到的所有应答
void dg_cli(FILE *fp, int sockfd, const sockaddr * pservaddr, socklen_t servlen)
{
    int n;
    const int on = 1;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    socklen_t len;
    struct sockaddr *preply_addr;

    preply_addr = Malloc(servlen);

    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    Signal(SIGALRM, recvfrom_alarm);

/*
   正确示范2   use sigsetjmp and siglongjmp
两个潜在的时序问题：1.信号在recvfrom返回和把它的返回值存入n之间被递交，那么会发生什么现象。该数据报将被认为已丢失，
不过UDP应用程序通常会处理数据报的丢失，然而同样的技术用于TCP应用程序，数据就永远丢失了，因为TCP已经确认了这个数据并把它递送给了应用进程。
dgclibast5也存在这个问题，该问题可通过在select返回之后关掉alarm来解决，另一个解决办法是使用select的定时功能
2.alarm调用和首次sigsetjmp调用之间的时间无法保证小于alarm时间。
*/
    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        alarm(5);  // start timer
        for(;;)
        {
            if(sigsetjmp(jmpbuf, 1) != 0)
            {
                break;
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
    siglongjmp(jmpbuf, 1);
}