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

// #define TEST_CONN_SENDTO
void dg_cli(FILE *fp, int sockfd, const sockaddr *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    struct sockaddr_in testaddr;
    bzero(&testaddr, sizeof(testaddr));
    testaddr.sin_family = AF_INET;
    testaddr.sin_port = htons(12345);
    Inet_pton(AF_INET, "172.16.3.253", &testaddr.sin_addr);

    Connect(sockfd, (sockaddr *)pservaddr, servlen);
    
#ifdef TEST_CONN_SENDTO
    int f = 1;
#endif
    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
#ifdef TEST_CONN_SENDTO        
        if(f)
        {
#endif
            Write(sockfd, sendline, strlen(sendline));

#ifdef TEST_CONN_SENDTO            
            f = 0;
        }
        else
        {
            // 在Ubuntu 16.04.3 LTS环境, 对connect过的udp套接字调用sendto函数并不会引发错误
            Sendto(sockfd, sendline, strlen(sendline), 0, (sockaddr *)&testaddr, sizeof(testaddr));
        }
#endif
        n = Read(sockfd, recvline, MAXLINE);

        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}