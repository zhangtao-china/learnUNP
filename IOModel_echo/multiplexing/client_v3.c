#include <string.h>
#include <unistd.h>
#include "../../base/wrapsocket.h"
#include "../../base/error.h"
#include "../../base/commondef.h"
#include "../../base/wrapio.h"
#include "../../base/wrapunix.h"

void str_cli(FILE *fp, int);

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        err_quit("usage: tcpcli <IPaddress>");
    }

    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    char localstr[SOCKADDR_STR_BUF_LEN];
    struct sockaddr_in servaddr, localaddr;
    socklen_t addrlen = sizeof(localaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr.s_addr);

    Connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
    
    if(Getsockname(sockfd, (sockaddr *)&localaddr, &addrlen) == 0)
    {
        if(sock_ntop((sockaddr *)&localaddr, addrlen, localstr, sizeof(localstr)) != NULL)
        {
            printf("[client_v3] local socket %s, connect to server: %s:%hu successfully.\n", 
                    localstr, argv[1], SERV_PORT);
        }
    }

    str_cli(stdin, sockfd);

    return 0;
}

void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];

    int n = 0;
    int isfd = fileno(fp);
    int maxfd = isfd > sockfd ? fileno(fp) + 1 : sockfd + 1;
    fd_set rdset;
    for(;;)
    {
        FD_ZERO(&rdset);
        FD_SET(sockfd, &rdset);
        FD_SET(isfd, &rdset);

        Select(maxfd, &rdset, NULL, NULL, NULL);

        if(FD_ISSET(isfd, &rdset))
        {
            if(Fgets(sendline, MAXLINE, fp) == NULL)
            {
                return;
            }
            Writen(sockfd, sendline, strlen(sendline));
        }

        if(FD_ISSET(sockfd, &rdset))
        {
            /*
             * 问题:
             * 对于批量输入, 很可能还有请求在去往服务器的路上, 或仍有应答在返回客户的路上
            */
            if( (n = Readline(sockfd, recvline, MAXLINE)) == 0)
            {
                err_quit("[client_v3] str_cli: server terminated prematurely");
            }

            Fputs(recvline, stdout);
        }
    }
}