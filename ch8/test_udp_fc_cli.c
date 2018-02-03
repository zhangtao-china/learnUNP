/*
   测试UDP缺乏流量控制的客户端程序
*/


#include <strings.h>
#include <stdio.h>
#include <string.h>
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/commondef.h"

#define NDG 2000
#define DGLEN 2200000

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
    int i;
    char sendline[DGLEN];

    for(i = 0; i < NDG; i++)
    {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
    }
}