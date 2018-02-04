#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include "../base/error.h"
#include "../base/wrapsocket.h"

int main(int argc, char **argv)
{
    char *ptr, **pptr;
    char str[INET_ADDRSTRLEN];

    struct hostent *hptr;

    while(--argc > 0)
    {
        ptr = *++argv;
        if( (hptr = gethostbyname(ptr)) == NULL)
        {
            err_msg("gethostbyname error for host: %s: %s", ptr, hstrerror(h_errno));
            continue;
        }

        printf("official hostname: %s\n", hptr->h_name);

        for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
        {
            printf("\talias: %s\n", *pptr);
        }

        switch(hptr->h_addrtype)
        {
            case AF_INET:
            {
                pptr = hptr->h_addr_list;
                for(; *pptr != NULL; pptr++)
                {
                    printf("\naddress: %s\n", Inet_ntop(hptr->h_addrtype, *pptr,
                            str, sizeof(str)));
                }
            }
            break;
            default:
            {
                err_ret("unknown address type");
            }
            break;
        }
    }

    struct in_addr addr;
    Inet_pton(AF_INET, "192.168.145.128", &addr);
    if( (hptr = gethostbyaddr(&addr, sizeof(addr), AF_INET)) != NULL)
    {
        printf("host name is: %s\n", hptr->h_name);
    }
    else
    {
        err_msg("gethostbyaddr error: %s", hstrerror(h_errno));
    }


    return 0;
}
