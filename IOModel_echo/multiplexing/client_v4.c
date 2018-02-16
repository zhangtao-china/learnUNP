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
/*
    Inet_pton(AF_INET, "192.168.145.128", &localaddr.sin_addr.s_addr); 
    localaddr.sin_port = 8888;
    localaddr.sin_family = AF_INET;
    Bind(sockfd, (sockaddr *)&localaddr, addrlen);
*/

    Connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
    
    if(Getsockname(sockfd, (sockaddr *)&localaddr, &addrlen) == 0)
    {
        if(sock_ntop((sockaddr *)&localaddr, addrlen, localstr, sizeof(localstr)) != NULL)
        {
            printf("[client_v4] local socket %s, connect to server: %s:%hu successfully.\n", 
                    localstr, argv[1], SERV_PORT);
        }
    }

    str_cli(stdin, sockfd);

    return 0;
}

void str_cli(FILE *fp, int sockfd)
{
    char buf[MAXLINE];

    int n = 0, stdineof = 0;
    int isfd = fileno(fp);
    int maxfd = isfd > sockfd ? fileno(fp) + 1 : sockfd + 1;
    fd_set rdset;
    for(;;)
    {
        FD_ZERO(&rdset);
        FD_SET(sockfd, &rdset);
        if(stdineof == 0)
        {
            FD_SET(isfd, &rdset);
        }

        Select(maxfd, &rdset, NULL, NULL, NULL);

        if(FD_ISSET(isfd, &rdset))
        {
            if((n = Read(isfd, buf, MAXLINE)) == 0)
            {
                stdineof = 1;
                Shutdown(sockfd, SHUT_WR);
                FD_CLR(isfd, &rdset);
                continue;
            }

            Writen(sockfd, buf, n);
        }

        if(FD_ISSET(sockfd, &rdset))
        {
            if((n = Readline(sockfd, buf, MAXLINE)) == 0)
            {
                if(stdineof == 1)
                {
                    return;
                }
                else
                {
                    err_quit("[client_v4] str_cli: server terminated prematurely.");
                }
            }

            Write(isfd, buf, n);
        }
    }
}