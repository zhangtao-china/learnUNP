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

void str_echo(int sockfd);
void sig_chld(int signo);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char clistr[SOCKADDR_STR_BUF_LEN];

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

/*
解决上一版本中未处理僵死进程的问题。
捕获SIGCHLD信号，处理僵死进程。
*/
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
                err_sys("[server_v2] accept error");
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
            printf("[server_v2] accepted client %s, start child process, pid is %d.\n", clistr, childpid);
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

void sig_chld(int signo)
{
    pid_t pid;
    int stat = -1;

/*
可以使用wait和waitpid来处理已终止的子进程。
正确的方法是使用waitpid而不是wait。wait的问题在于，如果有多个子进程终止，那么在信号处理函数执行之前将产生多个
信号，而信号函数只能执行一次，如果在循环中调用wait则没有办法防止wait在正运行的子进程尚未有终止时阻塞。
使用waitpid函数可解决此问题，第一个参数填-1表示等待第一个终止的子进程，第三个参数填WNOHANG，他告知内核在没有已终止
子进程时不要阻塞。
*/
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        printf("[server_v2] child %d terminated, result code is %d.\n", pid, stat);
    }
}