#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "../base/error.h"
#include "../base/wrapsocket.h"

#define BUFF_LEN (1024)
#define EXITSTR "q"

static void searchaddrinfo(const char *hostname);

int main()
{
    char inputs[BUFF_LEN] = {};

    printf("input Domain Name or input \"%s\" to exit: ", EXITSTR);

    size_t len = 0;
    while(fgets(inputs, BUFF_LEN, stdin) != NULL)
    {
        len = strlen(inputs);
        if(len < BUFF_LEN - 1)
        {
            if(inputs[strlen(inputs) - 1] == '\n')
            {
                inputs[strlen(inputs) - 1] = '\0';
            }
        }
        else
        {
            printf("input name is too long\n");
        }


        if(strcmp("q", inputs) == 0)
        {
            break;
        }

        searchaddrinfo(inputs);

        printf("input Domain Name or input \"%s\" to exit: ", EXITSTR);
    }

    return 0;
}

static void searchaddrinfo(const char *hostname)
{
    struct addrinfo *res = NULL;
    struct addrinfo *resref;
    char straddr[BUFF_LEN] = {};

    int ret = getaddrinfo(hostname, "https", NULL, &res);

    if(0 != ret)
    {
        printf("getaddrinfo error: %s\n", gai_strerror(ret));
        return ;
    }

    resref = res;

    int num = 1;
    while(resref != NULL)
    {
        if(NULL == sock_ntop(res->ai_addr, res->ai_addrlen, straddr, BUFF_LEN))
        {
            printf("addrinfo [%d]: invalid addr\n", num);
        }
        else
        {
            printf("addrinfo [%d]:\n", num);

            if(resref->ai_canonname != NULL)
            {
                printf("\tcanonical name: %s\n", resref->ai_canonname);
            }

            printf("\tip address: %s\n", straddr);
            printf("\tfamily=%d (%d:AF_INET, %d:AF_INET6, %d:AF_LOCAL, %d:AF_ROUTE, %d:AF_KEY)\n",
                   resref->ai_family, AF_INET, AF_INET6, AF_LOCAL, AF_ROUTE, AF_KEY);
            printf("\tsocktype=%d (%d:SOCK_STREAM, %d:SOCK_DGRAM, %d:SOCK_SEQPACKET, %d:SOCK_RAW)\n",
                   resref->ai_socktype, SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW);
            printf("\tprotocol=%d (%d:IPPROTO_TCP, %d:IPPROTO_UDP, %d:IPPROTO_SCTP)\n", 
                   resref->ai_protocol, IPPROTO_TCP, IPPROTO_UDP, IPPROTO_SCTP);
        }

        num++;
        resref = resref->ai_next;
    }

    freeaddrinfo(res);
}
