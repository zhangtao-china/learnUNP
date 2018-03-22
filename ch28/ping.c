#include <unistd.h>
#include <netinet/in.h>
#include "../base/error.h"
#include "../base/wrapunix.h"
#include "../base/wrapsocket.h"
#include "../base/wrapio.h"
#include "ping.h"

struct proto proto_v4 = {proc_v4, send_v4, NULL, NULL, NULL, 0, IPPROTO_ICMP};

#ifdef IPV6
struct proto proto_v6 = {proc_v6, send_v6, init_v6, NULL, NULL, 0, IPPROTO_ICMPV6};
#endif

int datalen = 56;

int main(int argc, char **argv)
{
    int c;
    struct addrinfo *ai;
    char *h;

    opterr = 0;
    // while((c = getopt(argc, argv, "v")) != 1)
    // {
    //     switch(c)
    //     {
    //     case 'v':
    //         verbose++;
    //         break;
    //     case '?':
    //         err_quit("unrecognized option: %c", c);
    //     }
    // }

    // if(optind != argc - 1)
    // {
    //     err_quit("usage: ping [-v] <hostname>");
    // }

    // host = argv[optind];
    host = argv[1];

    pid = getpid() & 0xffff;
    Signal(SIGALRM, sig_alrm);

    ai = host_serv(host, NULL, 0, 0);

    h = sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
    printf("PING %s (%s): %d data bytes\n", ai->ai_canonname ? ai->ai_canonname : h, h, datalen);

    if(ai->ai_family == AF_INET)
    {
        pr = &proto_v4;
#ifdef IPV6
    }
    else if(ai->ai_family == AF_INET6)
    {
        pr = &proto_v6;
        if(IN6_IS_ADDR_V4MAPPED(&(((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr)))
        {
            err_quit("cannot ping IPv4-mapped IPv6 address");
        }
#endif
    }
    else
    {
        err_quit("unknown addres family %d", ai->ai_family);
    }

    pr->sasend = ai->ai_addr;
    pr->sarecv = Calloc(1, ai->ai_addrlen);
    pr->salen = ai->ai_addrlen;

    readloop();

    return 0;
}
void readloop(void)
{
    int size;
    char recvbuf[BUFSIZE];
    char controlbuf[BUFSIZE];
    struct msghdr msg;
    struct iovec iov;
    ssize_t n;
    struct timeval tval;

    sockfd = Socket(pr->sasend->sa_family, SOCK_RAW, pr->icmpproto);
    setuid(getuid());
    if(pr->finit)
    {
        (*pr->finit)();
    }

    size = 60 *1024;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    sig_alrm(SIGALRM);

    iov.iov_base = recvbuf;
    iov.iov_len = sizeof(recvbuf);
    msg.msg_name = pr->sarecv;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = controlbuf;
    for(;;)
    {
        msg.msg_namelen = pr->salen;
        msg.msg_controllen = sizeof(controlbuf);
        n = recvmsg(sockfd, &msg, 0);
        if(n < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                err_sys("recvmsg error");
            }
        }

        Gettimeofday(&tval, NULL);
        (*pr->fproc)(recvbuf, n, &msg, &tval);
    }
}

void tv_sub(struct timeval *out, struct timeval *in)
{
    if((out->tv_usec -= in->tv_usec) < 0)
    {
        --out->tv_sec;
        out->tv_usec += 1000000;
    }

    out->tv_sec -= in->tv_sec;
}

void proc_v4(char *ptr, ssize_t len, struct msghdr *msg, struct timeval *tvrecv)
{
    int hlen1, icmplen;
    double rtt;
    struct ip *ip;
    struct icmp *icmp;
    struct timeval *tvsend;

    ip = (struct ip *)ptr;
    hlen1 = ip->ip_hl << 2;
    if(ip->ip_p != IPPROTO_ICMP)
    {
        return;
    }

    icmp = (struct icmp *)(ptr + hlen1);
    if((icmplen = len - hlen1) < 8)
    {
        return;
    }

    if(icmp->icmp_type == ICMP_ECHOREPLY)
    {
        if(icmp->icmp_id != pid)
        {
            return;
        }
        if(icmplen < 16)
        {
            return;
        }

        tvsend = (struct timeval *)(icmp->icmp_data);
        tv_sub(tvrecv, tvsend);
        rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0;

        printf("%d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms\n", icmplen, sock_ntop_host(pr->sarecv, pr->salen),
                icmp->icmp_seq, ip->ip_ttl, rtt);
    }
    else if(verbose)
    {
        printf("  %d bytes from %s: type = %d, code = %d\n", icmplen, sock_ntop_host(pr->sarecv, pr->salen),
               icmp->icmp_type, icmp->icmp_code);
    }
}

void init_v6()
{
#ifdef IPV6
    int on = 1;
    if(verbose == 0)
    {
        struct icmp6_filter myfilt;
        ICMP6_FILTER_SETBLOCKALL(&myfilt);
        ICMP6_FILTER_SETPASS(ICMP6_ECHO_REPLY, &myfilt);
        setsockopt(sockfd, IPPROTO_IPV6, ICMP6_FILTER, &myfilt, sizeof(myfilt));
    }

#ifdef IPV6_RECVHOPLIMIT
    setsockopt(sockfd, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &on, sizeof(on));
#else
    setsockopt(sockfd, IPPROTO_IPV6, IPV6_HOPLIMIT, &on, sizeof(on));
#endif

#endif
}

void proc_v6(char *ptr, ssize_t len, struct msghdr *msg, struct timeval *tvrecv)
{
#ifdef IPV6
    double rtt;
    struct icmp6_hdr *icmp6;
    struct timeval *tvsend;
    struct cmsghdr *cmsg;
    int hlim;

    icmp6 = (struct icmp6_hdr *)ptr;
    if(len < 8)
    {
        return;
    }

    if(icmp6->icmp6_type == ICMP6_ECHO_REPLY)
    {
        if(icmp6->icmp6_id != pid)
        {
            return;
        }
        if(len < 16)
        {
            return; 
        }

        tvsend = (struct timeval *)(icmp6 + 1);
        tv_sub(tvrecv, tvsend);
        rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0;

        hlim = -1;
        for(cmsg = CMSG_FIRSTHDR(msg); cmsg != NULL; cmsg = CMSG_NXTHDR(msg, cmsg))
        {
            if(cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_HOPLIMIT)
            {
                hlim = *(u_int32_t *)CMSG_DATA(cmsg);
                break;
            }
        }

        printf("%d bytes from %s: seq=%u, hlim=", len, sock_ntop_host(pr->sarecv, pr->salen), icmp6->icmp6_seq);

        if(hlim == -1)
        {
            printf("???");
        }
        else
        {
            printf("%d", hlim);
        }
        printf(", rtt=%.3f ms\n", rtt);
    }
    else if(verbose)
    {
        printf("  %d bytes from %s: type = %d, code = %d\n", len, sock_ntop_host(pr->sarecv, pr->salen),
               icmp6->icmp6_type, icmp6->icmp6_code);
    }
#endif
}

void sig_alrm(int signo)
{
    (*pr->fsend)();

    alarm(1);
    return;
}

uint16_t in_cksum(uint16_t *addr, int len)
{
    int nleft = len;
    uint32_t sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;

    while(nleft > 1)
    {
        sum += *w++;
        nleft -= 2;
    }

    if(nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}

void send_v4(void)
{
    int len;
    struct icmp *icmp;

    icmp = (struct icmp *)sendbuf;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_id = pid;
    icmp->icmp_seq = nsent++;
    memset(icmp->icmp_data, 0xa5, datalen);
    Gettimeofday((struct timeval *)icmp->icmp_data, NULL);

    len = 8 + datalen;
    icmp->icmp_cksum = 0;
    icmp->icmp_cksum = in_cksum((ushort *)icmp, len);

    Sendto(sockfd, sendbuf, len, 0, pr->sasend, pr->salen);
}

void send_v6()
{
#ifdef IPV6
    int len;
    struct icmp6_hdr *icmp6;

    icmp6 = (struct icmp6_hdr *)sendbuf;
    icmp6->icmp6_type = ICMP6_ECHO_REQUEST;
    icmp6->icmp6_code = 0;
    icmp6->icmp6_id = pid;
    icmp6->icmp6_seq = nsent++;
    memset((icmp6 + 1), 0xa5, datalen);
    Gettimeofday((struct timeval *)(icmp6 + 1), NULL);

    len = 8 + datalen;

    Sendto(sockfd, sendbuf, len, 0, pr->sasend, pr->salen);
#endif
}