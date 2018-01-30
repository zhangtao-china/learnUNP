#include <strings.h>
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "../base/error.h"
#include "../base/commondef.h"

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

void dg_echo(int sockfd, sockaddr *pcliaddr, socklen_t clilen)
{
    int n;
    socklen_t len;
    char msg[MAXLINE];
    char strcli[SOCKADDR_STR_BUF_LEN];

    for(;;)
    {
        len = clilen;
        n = Recvfrom(sockfd, msg, MAXLINE, 0, pcliaddr, &len);

        if(sock_ntop(pcliaddr, len, strcli, sizeof(strcli)))
        {
            printf("[server_udp_v1] receive message from %s", strcli);
        }

        Sendto(sockfd, msg, n, 0, pcliaddr, len);
    }
}