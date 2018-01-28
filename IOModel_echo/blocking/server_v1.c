#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <stdlib.h>
#include "../../base/wrapsocket.h"
#include "../../base/error.h"
#include "../../base/commondef.h"
#include "../../base/wrapio.h"
#include "../../base/wrapunix.h"

void str_echo(int sockfd);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char clistr[INET_ADDRSTRLEN];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

/*
存在的问题：
在子进程终止后，父进程未捕获SIGCHLD信号，而该信号默认行为是被忽略。由于父进程未加处理，
于是子进程进入僵死状态。僵死进程占用内核空间，最终可能会导致耗尽进程资源。因此必须清理它们
*/
    for(;;)
    {
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (sockaddr*)&cliaddr, &clilen);

        if((childpid = Fork()) == 0)
        {
            Close(listenfd);
            str_echo(connfd);

            exit(0);
        }

        if(Inet_ntop(AF_INET, &cliaddr.sin_addr, clistr, sizeof(clistr)) != NULL)
        {
            printf("[server_v1] accepted client %s:%hu, start child process, pid is %d.\n", clistr, cliaddr.sin_port, childpid);
        }

        Close(connfd);
    }
}


void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];

again:
    while((n = read(sockfd, buf, MAXLINE)) > 0)
    {
        Writen(sockfd, buf, n);
    }

    if(n < 0 && errno == EINTR)
    {
        goto again;
    }
    else
    {
        err_sys("[server_v2] str_echo: read error");
    }
}