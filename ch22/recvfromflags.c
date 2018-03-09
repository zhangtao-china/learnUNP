#include <sys/param.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <strings.h>
#include <string.h>
#include "../base/wrapsocket.h"


struct unp_in_pktinfo
{
    struct in_addr ipi_addr;
    int            ipi_ifindex;
};

ssize_t recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
                       sockaddr * sa, socklen_t *salenptr, struct unp_in_pktinfo *pktp)
{
    struct msghdr msg;
    struct iovec iov[1];
    ssize_t n;
    struct cmsghdr *cmptr;

    union
    {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(struct in_addr)) + CMSG_SPACE(sizeof(struct unp_in_pktinfo))];
    }control_un;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    msg.msg_flags = 0;

    msg.msg_name = sa;
    msg.msg_namelen = *salenptr;
    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if((n = recvmsg(fd, &msg, *flagsp)) < 0)
    {
        return n;
    }

    *salenptr = msg.msg_namelen;
    if(pktp)
    {
        bzero(pktp, sizeof(struct unp_in_pktinfo));
    }

    *flagsp = msg.msg_flags;
    if(msg.msg_controllen < sizeof(struct cmsghdr) || (msg.msg_flags & MSG_CTRUNC) || pktp == NULL)
    {
        return n;
    }

    for(cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL; cmptr = CMSG_NXTHDR(&msg, cmptr))
    {
#ifdef IP_RECVDSTADDR
        if(cmptr->cmsg_level == IPPROTO_IP && cmptr->cmsg_type == IP_RECVDSTADDR)
        {
            memcpy(&pktp->ipi_addr, CMSG_DATA(cmptr), sizeof(struct in_addr));
            continue;
        }
#endif
#ifdef IP_RECVIF
        if(cmptr->cmsg_level == IPPRORO_IP && cmptr->cmsg_type == IP_RECVIF)
        {
            struct sockaddr_dl *sdl;
            sdl = (struct sockaddr_dl *)CMSG_DATA(cmptr);
            pktp->ipi_ifindex = sdl->sdl_index;
            continue;
        }
#endif
    }

    return n;
}