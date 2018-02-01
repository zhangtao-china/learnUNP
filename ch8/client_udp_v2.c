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

    struct sockaddr_storage replyaddr;
    socklen_t len;
    char buf[SOCKADDR_STR_BUF_LEN];
    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

/*
问题：
如果服务器是多宿的，该客户可能失败
两个解决办法：
1.得到由recvfrom返回的IP地址后，客户通过DNS中查找服务器的名字来验证该主机的域名
2.UDP服务器给服务器主机上配置的每个IP地址创建一个套接字，用bind捆绑每个IP地址到各自的套接字，然后在
  所有这些套接字上使用select，再从可读的套接字给出应答
*/
        len = servlen;
        n = Recvfrom(sockfd, recvline, MAXLINE, 0, (sockaddr *)&replyaddr, &len);

        if(len != servlen || memcmp(pservaddr, &replyaddr, len) != 0)
        {
            printf("reply from %s (ignored)\n", 
                   sock_ntop((sockaddr *)&replyaddr, len, buf, sizeof(buf)));
            continue;
        }

        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}