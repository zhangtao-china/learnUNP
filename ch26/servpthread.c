#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/wrappthread.h"
#include "../base/wrapunix.h"

static void * doit(void *arg);

int main(int argc, char **argv)
{
    int listenfd, *iptr;
    pthread_t tid;
    socklen_t addrlen, len;
    sockaddr *cliaddr;

    if(argc == 2)
    {
        listenfd = tcp_listen(NULL, argv[1], &addrlen);
    }
    else if(argc == 3)
    {
        listenfd = tcp_listen(argv[1], argv[2], &addrlen);
    }
    else
    {
        err_quit("usage: servpthread [ <host> ] <service or port> ");
    }

    cliaddr = Malloc(addrlen);

    for(;;)
    {
        len = addrlen;
        iptr = Malloc(sizeof(int));
        *iptr = Accept(listenfd, cliaddr, &len);
        Pthread_create(&tid, NULL, &doit, iptr);
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
        err_sys("str_echo: read error");
    }
}

static void * doit(void *arg)
{
    int connfd;

    connfd = *((int *)arg);
    free(arg);

    Pthread_detach(pthread_self());
    str_echo(connfd);
    Close(connfd);
    return NULL;
}