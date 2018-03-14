#include <unistd.h>
#include "../base/wrapunix.h"
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"

int listenfd, connfd;

void sig_urg(int);

int main(int argc, char **argv)
{
    int size;;
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
        err_quit("usage: tcprecv05 [ <host> ] <port#>");
    }

    size = 4096;
    Setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    connfd = Accept(listenfd, NULL, NULL);

    Signal(SIGURG, sig_urg);

    Fcntl(connfd, F_SETOWN, getpid());

    for(;;)
    {
        pause();
    }
}

void sig_urg(int signo)
{
    int n;
    char buff[2048];
/*
err_sys函数先出的出错消息对应于EAGAIN，EAGAIN等同于FreeBSD中的EWOULDBLOCK。发送端TCP向接收端TCP发送了带外通知，由此产生递交给接收进程
的SIGURG信号。然而当接收进程指定MSG_OOB标志调用recv时，相应带外字节不能读入。
解决办法是让接收进程通过读入已排队的普通数据，在套接字接收缓冲区腾出空间。浙江刀子接收端TCP向发送端通告一个非零窗口，最终允许发送端发送带外字节
*/
    printf("SIGURG received\n");
    n = Recv(connfd, buff, sizeof(buff) - 1, MSG_OOB);
    buff[n] = 0;
    printf("read %d OOB byte\n", n);
}