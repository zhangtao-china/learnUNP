#include <unistd.h>
#include "../base/wrapsocket.h"
#include "../base/error.h"
#include "../base/wrapio.h"

#define WILL_SLEEP

int main(int argc, char **argv)
{
    int sockfd;

    if(argc != 3)
    {
        err_quit("usage: tcpsend01 <host> <port#>");
    }

    sockfd = tcp_connect(argv[1], argv[2]);

    Write(sockfd, "123", 3);
    printf("wrote 3 bytes of normal data\n");
#ifdef WILL_SLEEP
    sleep(1);
#endif

/*
如果使用MSG_OOB标志发送了多于1个字节的数据，则只有最后的一个字节才会被认为是带外数据，前面的若干字节被当做正常数据发送
*/
    // Send(sockfd, "4abc", 4, MSG_OOB);
    Send(sockfd, "4", 4, MSG_OOB);
    printf("wrote 1 byte of OOB data\n");
#ifdef WILL_SLEEP
    sleep(1);
#endif

    Write(sockfd, "56", 2);
    printf("wrote 2 bytes of normal data\n");
#ifdef WILL_SLEEP
    sleep(1);
#endif

    Send(sockfd, "7", 1, MSG_OOB);
    printf("wrote 1 byte of OOB data\n");
#ifdef WILL_SLEEP
    sleep(1);
#endif

    Write(sockfd, "89", 2);
    printf("wrote 2 bytes of normal data\n");
#ifdef WILL_SLEEP
    sleep(1);
#endif

    return 0;
}