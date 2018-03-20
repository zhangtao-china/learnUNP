#include <strings.h>
#include "../../base/wrapsocket.h"
#include "../../base/wrapio.h"
#include "../../base/error.h"
#include "../../base/commondef.h"
#include "../../base/wrapunix.h"


/*
对一个套接字使用信号驱动式I/O(SIGIO)要求进程执行以下3个步骤：
1.建立SIGIO信号的信号处理函数
2.设置该套接字的属性，通常使用fcntl的F_SETOWN命令设置
3.开启该套接字的信号驱动式I/O，通常通过fcntl的F_SETFL命令打开O_ASYNC标志完成。
O_ASYNC标志是相对较晚加到POSIX规范中的。通常还可使用ioctl的FIOASYNC请求代为开启信号驱动式I/O
*/

#define QSIZE 8
#define MAXDG 4096

void dg_echo(int sockfd, sockaddr *pcliaddr, socklen_t clilen);

static int sockfd;

typedef struct
{
    void *dg_data;
    size_t dg_len;
    sockaddr *dg_sa;
    socklen_t dg_salen;
}DG;

DG dg[QSIZE];
static long cntread[QSIZE + 1];

static int iget;
static int iput;
static int nqueue;
static socklen_t clilen;

static void sig_io(int);
static void sig_hup(int);

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(sockfd, (sockaddr *)&servaddr, sizeof(servaddr));

    dg_echo(sockfd, (sockaddr *)&cliaddr, sizeof(cliaddr));
}

void dg_echo(int sockfd_arg, sockaddr *pcliaddr, socklen_t clilen_arg)
{
    int i;
    int on = 1;
    sigset_t zeromask, newmask, oldmask;

    sockfd = sockfd_arg;
    clilen = clilen_arg;

    for(i = 0; i < QSIZE; ++i)
    {
        dg[i].dg_data = Malloc(MAXDG);
        dg[i].dg_sa = Malloc(clilen);
        dg[i].dg_salen = clilen;
    }

    iget = iput = nqueue = 0;

    Signal(SIGHUP, sig_hup);
    Signal(SIGIO, sig_io);
    Fcntl(sockfd, F_SETOWN, getpid());
    Ioctl(sockfd, FIOASYNC, &on);
    Ioctl(sockfd, FIONBIO, &on);

    Sigemptyset(&zeromask);
    Sigemptyset(&oldmask);
    Sigemptyset(&newmask);
    Sigaddset(&newmask, SIGIO);

    Sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    for(;;)
    {
/*
如果测试nqueue这个由主循环和信号处理函数共享的变量时SIGIO未被阻塞，那么可能测试nqueue时发现它为0，但是刚测试完毕SIGIO信号就递交了，
导致nqueue被设置为1。接着调用sigsuspend进入睡眠，这样实际上就错过了这个信号，否则将永远不能从sigsuspend调用中被唤醒。 
这一点类似于race condition
*/
        while(nqueue == 0)
        {
            sigsuspend(&zeromask);
        }

        Sigprocmask(SIG_SETMASK, &oldmask, NULL);

        Sendto(sockfd, dg[iget].dg_data, dg[iget].dg_len, 0, dg[iget].dg_sa, dg[iget].dg_salen);

        if(++iget >= QSIZE)
        {
            iget = 0;
        }

/*
在测试nqueue和递减nqueue时都需要阻塞SIGIO，可以去掉for循环内的两个sigprocmask调用。然而这么做的问题是执行整个循环期间SIGIO一直阻塞着，
从而降低了信号处理函数的及时性。数据报不应该又因为如此变动而丢失，但是SIGIO信号向进程的递交将在整个阻塞期间一直被拖延。
编写执行信号处理的程序时，努力目标之一应该是尽可能的减少阻塞信号的时间
*/
        Sigprocmask(SIG_BLOCK, &newmask, &oldmask);
        nqueue--;
    }
}

static void sig_io(int signo)
{
    ssize_t len;
    int nread;
    DG *ptr;

/*
使用for循环的意义：POSIX信号通常不排队，如果在信号处理函数中执行，期间该信号又发生了2次，那么它实际只被递交1次。
这种情况下将开启信号驱动式IO的描述符通常也被设置为非阻塞式，把SIGIO信号处理函数编写成在一个循环中执行读入操作，
直到该操作返回EWOULDBLOCK时才结束循环
*/
    for(nread = 0; ;)
    {
        if(nqueue >= QSIZE)
        {
            err_quit("receive overflow");
        }

        ptr = &dg[iput];
        ptr->dg_salen = clilen;
        len = recvfrom(sockfd, ptr->dg_data, MAXDG, 0, ptr->dg_sa, &ptr->dg_salen);

        if(len < 0)
        {
            if(errno == EWOULDBLOCK)
            {
                break;
            }
            else
            {
                err_sys("recvfrom error");
            }
        }

        ptr->dg_len = len;

        nread++;
        nqueue++;
        if(++iput >= QSIZE)
        {
            iput = 0;
        }
    }

    cntread[nread]++;
}

static void sig_hup(int signo)
{
    int i;

    for(i = 0; i <= QSIZE; ++i)
    {
        printf("cntread[%d] = %ld\n", i, cntread[i]);
    }
}
