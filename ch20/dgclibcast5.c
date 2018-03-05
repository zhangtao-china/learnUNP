#include <string.h>
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/wrapunix.h"
#include "../base/commondef.h"

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

static int pipefd[2];

// 设置SO_BROADCAST套接字选项并显示在5s内收到的所有应答
void dg_cli(FILE *fp, int sockfd, const sockaddr * pservaddr, socklen_t servlen)
{
    int n, maxfdp1;
    const int on = 1;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    fd_set rset;
    socklen_t len;
    struct sockaddr *preply_addr;

    preply_addr = Malloc(servlen);

    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    Pipe(pipefd);  // 也可以使用socketpair创建一个全双工管道
    maxfdp1 = max(sockfd, pipefd[0]) + 1;

    FD_ZERO(&rset);

    Signal(SIGALRM, recvfrom_alarm);

/*
   正确示范3   use IPC 
*/
    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        alarm(5);  // start timer
        for(;;)
        {
            FD_SET(sockfd, &rset);
            FD_SET(pipefd[0], &rset);
            n = select(sockfd+1, &rset, NULL, NULL, NULL);
            if(n < 0)
            {
                if(errno == EINTR)
                {
                    continue;
                }
                else
                {
                    err_sys("select error");
                }
            }
            
            if(FD_ISSET(sockfd, &rset))
            {
                len = servlen;
                n = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
                recvline[n] = 0;
                printf("from %s: %s", sock_ntop_host(preply_addr, len), recvline);
            }

            if(FD_ISSET(pipefd[0], &rset))
            {
                Read(pipefd[0], &n, 1);
                break;
            }
        }
    }

    free(preply_addr);
}


static void recvfrom_alarm(int signo)
{
    Write(pipefd[1], "", 1);

    return;
}