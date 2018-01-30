#include <strings.h>
#include <stdio.h>
#include <string.h>
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/commondef.h"

void dg_cli(FILE *fp, int sockfd, const sockaddr *pservaddr, socklen_t servlen);

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if(argc != 2)
    {
        err_quit("usage: tcpcli <IPaddress>");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    dg_cli(stdin, sockfd, (sockaddr *)&servaddr, sizeof(servaddr));
}

void dg_cli(FILE *fp, int sockfd, const sockaddr *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    sockaddr cliaddr;
    socklen_t clilen;
    char strcli[SOCKADDR_STR_BUF_LEN];
    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        n = Recvfrom(sockfd, recvline, MAXLINE, 0, (sockaddr *)&cliaddr, &clilen);

        if(sock_ntop(&cliaddr, clilen, strcli, sizeof(strcli)))
        {
            printf("[client_udp_v1] recv %d bytes from %s", n, strcli);
        }

        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}