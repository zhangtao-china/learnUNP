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
        err_quit("usage: dgclibcast1 <IPaddress>");
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

    preply_addr = Malloc(servlen);

    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    Signal(SIGALRM, recvfrom_alarm);

/*
存在的问题：
alarm的目的是让recvfrom返回,
然而SIGALRM信号可能在任何时候递交，假如我们在从读取最后一个应答的recvfrom返回并在进入下一次循环期间信号被递交，
那么我们将会永远阻塞在recvfrom调用(如果没有其他任何应答)
*/
    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        alarm(5);  // start timer
        for(;;)
        {
            len = servlen;
            n = recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
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