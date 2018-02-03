#include <strings.h>
#include <stdlib.h>
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/commondef.h"
#include "../base/wrapunix.h"

void dg_echo(int sockfd, sockaddr *pcliaddr, socklen_t clilen);

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

static void recvfrom_int(int);
static int count = 0;
void dg_echo(int sockfd, sockaddr *pcliaddr, socklen_t clilen)
{
    int n;
    socklen_t len;
    char msg[MAXLINE];

    Signal(SIGINT, recvfrom_int);

    for(;;)
    {
        len = clilen;
        Recvfrom(sockfd, msg, MAXLINE, 0, pcliaddr, &len);

        count++;
    }
}

static void recvfrom_int(int signo)
{
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}