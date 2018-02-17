#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/un.h>
#include "../base/wrapsocket.h"
#include "../base/error.h"
#include "../base/commondef.h"
#include "../base/wrapio.h"
#include "../base/wrapunix.h"

void str_echo(int sockfd);
void sig_chld(int signo);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_un cliaddr, servaddr;
    char clistr[SOCKADDR_STR_BUF_LEN];

    listenfd = Socket(AF_LOCAL, SOCK_STREAM, 0);

    unlink(UNIXSTR_PATH);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXSTR_PATH);

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    Signal(SIGCHLD, sig_chld);

    for(;;)
    {
        clilen = sizeof(cliaddr);
        if((connfd = Accept(listenfd, (sockaddr*)&cliaddr, &clilen)) < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                err_sys("[unixstrserv01] accept error");
            }
        }

        if((childpid = Fork()) == 0)
        {
            Close(listenfd);
            str_echo(connfd);

            exit(0);
        }

        if(sock_ntop((sockaddr *)&cliaddr, clilen, clistr, sizeof(clistr)) != NULL)
        {
            printf("[unixstrserv01] accepted client %s, start child process, pid is %d.\n", clistr, childpid);
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
        err_sys("[unixstrserv01] str_echo: read error");
    }
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat = -1;

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        printf("[unixstrserv01] child %d terminated, result code is %d.\n", pid, stat);
    }
}