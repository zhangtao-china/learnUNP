#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "../../base/wrapsocket.h"
#include "../../base/error.h"
#include "../../base/commondef.h"
#include "../../base/wrapio.h"
#include "../../base/wrapunix.h"

void sig_chld(int signo);
void str_echo(int sockfd);

int max(int lhs, int rhs)
{
    if(lhs >= rhs)
    {
        return lhs;
    }
    else
    {
        return rhs;
    }
}

int main(int argc, char **argv)
{
    int listenfd, connfd, udpfd, nready, maxfdp1;
    char mesg[MAXLINE];
    pid_t childpid;
    fd_set rset;
    ssize_t n;
    socklen_t len;
    const int on = 1;
    struct sockaddr_in cliaddr, servaddr;
    char clistr[SOCKADDR_STR_BUF_LEN];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));
    
    Listen(listenfd, LISTENQ);

    udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(udpfd, (sockaddr *)&servaddr, sizeof(servaddr));

    Signal(SIGCHLD, sig_chld);

    FD_ZERO(&rset);
    maxfdp1 = max(listenfd, udpfd) + 1;
    for(;;)
    {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);

        if((nready = select(maxfdp1 + 1, &rset, NULL, NULL, NULL)) < 0)
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

        if(FD_ISSET(listenfd, &rset))
        {
            len = sizeof(cliaddr);
            connfd = Accept(listenfd, (sockaddr*)&cliaddr, &len);

            if((childpid = Fork()) == 0)
            {
                Close(listenfd);
                str_echo(connfd);
                exit(0);
            }

            Close(connfd);

            if(sock_ntop((sockaddr *)&cliaddr, len, clistr, sizeof(clistr)) != NULL)
            {
                printf("[server_v5] accepted client %s, start child process, pid is %d.\n", clistr, childpid);
            }
        }

        if(FD_ISSET(udpfd, &rset))
        {
            len = sizeof(cliaddr);
            n = Recvfrom(udpfd, mesg, MAXLINE, 0, (sockaddr *)&cliaddr, &len);

            Sendto(udpfd, mesg, n, 0, (sockaddr *)&cliaddr, len);
        }
    }
}

void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];

    struct sockaddr_storage cliaddr;
    char clistr[SOCKADDR_STR_BUF_LEN];
    socklen_t clilen;

again:
    while((n = read(sockfd, buf, MAXLINE)) > 0)
    {
        Writen(sockfd, buf, n);
    }

    if(n < 0 && errno == EINTR)
    {
        goto again;
    }
    else if(n == 0)
    {
        clilen = sizeof(cliaddr);
        Getpeername(sockfd, (sockaddr *)&cliaddr, &clilen);
        if(sock_ntop((sockaddr *)&cliaddr, clilen, clistr, sizeof(clistr)) != NULL)
        {
            printf("[server_v4] connection %s closed by client.\n", clistr);
        }

        Close(sockfd);
    }
    else
    {
        err_sys("[server_v5] str_echo: read error");
    }
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat = -1;

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        printf("[server_v5] child %d terminated, result code is %d.\n", pid, stat);
    }
}