#include <sys/socket.h>
#include <sys/types.h>
#include <net/route.h>
#include <net/if_dl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFLEN (sizeof(struct rt_msghdr) + 521)

#define SEQ 9999

#define ROUNDUP(a, size) (((a) & ((size) - 1)) ? (1 + ((a) | ((size) - 1))) : (a))
#define NEXT_SA(ap) ap = (struct sockaddr *) \
                         ((caddr_t)ap + (ap->sa_len ? ROUNDUP(ap->salen, sizeof(u_long)) : sizeof(u_long)))

char * sock_ntop_host(const struct sockaddr *sa, socklen_t salen);
void get_rtaddrs(int addrs, struct sockaddr * sa, struct sockaddr **rti_info);
const char * sock_masktop(struct sockaddr *sa, socklen_t salen);

int main(int argc, char **argv)
{
    int sockfd, res;
    char *buf;
    pid_t pid;
    ssize_t n;
    struct rt_msghdr *rtm;
    struct sockaddr *sa, *rti_info[RTAX_MAX];

    if(argc != 2)
    {
        printf("usage: getrt <IPaddress>");
        return -1;
    }

    sockfd = socket(AF_ROUTE, SOCK_RAW, 0);
    if(sockfd < 0)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    buf = calloc(1, BUFLEN);
    if(buf == NULL)
    {
        printf("calloc error: %s\n", strerror(errno));
        return -1;
    }

    rtm = (struct rt_msghdr *)buf;
    rtm->rtm_msglen = sizeof(struct rt_msghdr) + sizeof(struct sockaddr_in);
    rtm->rtm_version = RTM_VERSION;
    rtm->rtm_type = RTM_GET;
    rtm->rtm_addrs = RTA_DST;
    rtm->rtm_pid = pid = getpid();
    rtm->rtm_seq = SEQ;

    sin = (struct sockaddr_in *)(rtm + 1);
    sin->sin_len = sizeof(struct sockaddr_in);
    sin->sin_family = AF_INET;
    res = inet_pton(AF_INET, argv[1], &sin->sin_addr);
    if(res <= 0 )
    {
        printf("inet_pton error: %s\n", strerror(errno));
        return -1;
    }

    if((res = write(sockfd, rtm, rtm->rtm_msglen)) != rtm->rtm_msglen)
    {
        printf("write error: %s\n", strerror(errno));
        return -1;
    }

    do
    {
        n = read(sockfd, rtm, BUFLEN);
    }while(rtm->rtm_type != RTM_GET || rtm->rtm_seq != SEQ || rtm->rtm_pid != pid);

    rtm = (struct rt_msghdr *)buf;
    sa = (struct sockaddr *)(rtm + 1);
    get_rtaddrs(rtm->rtm_addrs, sa, rti_info);
    if((sa = rti_info[RTAX_DST]) != NULL)
    {
        printf("dest: %s\n", sock_ntop_host(sa, sa->salen));
    }

    if((sa = rti_info[RTAX_GATEWAY]) != NULL)
    {
        printf("getway: %s\n", sock_ntop_host(sa,sa->sa_len));
    }

    if((sa = rti_info[RTAX_NETMASK]) != NULL)
    {
        printf("netmask: %s\n", sock_masktop(sa, sa->sa_len));
    }

    if(sa = rti_info[RTAX_GENMASK]) != NULL)
    {
        printf("genmask: %s\n", sock_masktop(sa, sa->sa_len));
    }

    return 0;
}

char * sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
    static char str[128];		 

	switch (sa->sa_family) 
    {
	case AF_INET: 
    {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return NULL;
		return str;
	}

#ifdef	IPV6
	case AF_INET6: 
    {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
			return NULL;
		return str;
	}
#endif

#ifdef	AF_UNIX
	case AF_UNIX: 
    {
		struct sockaddr_un	*unp = (struct sockaddr_un *) sa;

			/* OK to have no pathname bound to the socket: happens on
			   every connect() unless client calls bind() first. */
		if (unp->sun_path[0] == 0)
			strcpy(str, "(no pathname bound)");
		else
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		return str;
	}
#endif

#ifdef	HAVE_SOCKADDR_DL_STRUCT
	case AF_LINK:
    {
		struct sockaddr_dl	*sdl = (struct sockaddr_dl *) sa;

		if (sdl->sdl_nlen > 0)
			snprintf(str, sizeof(str), "%*s",
					 sdl->sdl_nlen, &sdl->sdl_data[0]);
		else
			snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
		return(str);
	}
#endif
	default:
		snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d",
				 sa->sa_family, salen);
		return(str);
	}

    return NULL;
}

void get_rtaddrs(int addrs, ;struct sockaddr * sa, struct sockaddr **rti_info)
{
    int i;
    for(i = 0; i < RTAX_MAX; i++)
    {
        if(addrs & (1 << i))
        {
            rti_info[i] = sa;
            NEXT_SA(sa);
        }
        else
        {
            rti_info[i] = NULL;
        }
    }
}

const char * sock_masktop(struct sockaddr *sa, socklen_t salen)
{
    static char str[INET6_ADDRSTRLEN];
    unsigned char *ptr = &sa->sa_data[2];

    if(sa->sa_len == 0)
    {
        return "0.0.0.0";
    }
    else if(sa->sa_len == 5)
    {
        snprintf(str, sizeof(str), "%d.0.0.0", *ptr);
    }
    else if(sa->sa_len == 6)
    {
        snprintf(str, sizeof(str), "%d.%d.0.0", *ptr, *(ptr + 1));
    }
    else if(sa->sa_len == 7)
    {
        snprintf(str, sizeof(str), "%d.%d.%d.0", *ptr, *(ptr + 1, *(ptr + 2)));
    }
    else if(sa->sa_len == 8)
    {
        snprintf(str, sizeof(str), "%d.%d.%d.%d", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3));
    }

    return str;
}