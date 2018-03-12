#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/wrapunix.h"

int listenfd, connfd;

void sig_urg(int);

int main(int argc, char **argv)
{
    int n;
    char buff[128];

    if(argc == 2)
    {
        listenfd = tcp_listen(NULL, argv[1], NULL);
    }
    else if(argc == 3)
    {
        listenfd = tcp_listen(argv[1], argv[2], NULL);
    }
    else
    {
        err_quit("usage: tcprecv01 [ <host> ] <port#>");
    }

    connfd = Accept(listenfd, NULL, NULL);
/*
直到accept返回后才建立信号处理函数。这么做会错过一些小概率出现的带外数据，它们在TCP完成三路握手之后但在accept返回之前到达。
然而如果我们在调用accept之前建立信号处理函数并设置监听套接字的属主(本属性将传承给已连接套接字)，那么如果带外数据在accept返回之前到达，
我们的信号出来函数将没有真正的connfd可用。
*/
    Signal(SIGURG, sig_urg);
    Fcntl(connfd, F_SETOWN, getpid());

    for(;;)
    {
        if((n = Read(connfd, buff, sizeof(buff))) == 0)
        {
            printf("received EOF\n");
            return 0;
        }

        buff[n] = 0;
        printf("read %d bytes: %s\n", n, buff);
    }

    return 0;
}

void sig_urg(int signo)
{
    int n;
    char buff[128];
/*
从信号处理函数中调用不安全的printf不被推荐。这样做只是为了查看程序在干什么
*/
    printf("-----------------------------------------------\n");
    printf("SIGURG received\n");
    n = Recv(connfd, buff, sizeof(buff), MSG_OOB);
    buff[n] = 0;

    printf("read %d OOB byte: %s\n", n, buff);
    printf("-----------------------------------------------\n");
}