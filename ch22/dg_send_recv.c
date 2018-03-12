#include <setjmp.h>
#include "rtt.h"
#include "../base/wrapsocket.h"
#include "../base/wrapunix.h"
#include "../base/error.h"
#include "../base/wrapio.h"

#define RTT_DEBUG

static struct rtt_info rttinfo;
static int rttinit = 0;
static struct msghdr msgsend, msgrecv;
static struct hdr
{
    uint32_t seq;
    uint32_t ts;
}sendhdr, recvhdr;

static void sig_alarm(int signo);
static sigjmp_buf jmpbuf;

static ssize_t dg_send_recv(int fd, const void *outbuff, size_t outbytes,
                     void *inbuff, size_t inbytes, const sockaddr *destaddr, socklen_t destlen)
{
    ssize_t n;
    struct iovec iovsend[2], iovrecv[2];

    if(rttinit == 0)
    {
        rtt_init(&rttinfo);
        rttinit = 1;
        rtt_d_flag = 1;
    }

    sendhdr.seq++;
    msgsend.msg_name = (void *)destaddr;
    msgsend.msg_namelen = destlen;
    msgsend.msg_iov = iovsend;
    msgsend.msg_iovlen = 2;
    iovsend[0].iov_base = &sendhdr;
    iovsend[0].iov_len = sizeof(struct hdr);
    iovsend[1].iov_base = (void *)outbuff;
    iovsend[1].iov_len = outbytes;

    msgrecv.msg_name = NULL;
    msgrecv.msg_namelen = 0;
    msgrecv.msg_iov = iovrecv;
    msgrecv.msg_iovlen = 2;
    iovrecv[0].iov_base = &recvhdr;
    iovrecv[0].iov_len = sizeof(struct hdr);
    iovrecv[1].iov_base = inbuff;
    iovrecv[1].iov_len = inbytes;

    Signal(SIGALRM, sig_alarm);
    rtt_newpack(&rttinfo);

sendagain:
    sendhdr.ts = rtt_ts(&rttinfo);

    Sendmsg(fd, &msgsend, 0);

    alarm(rtt_start(&rttinfo));
    if(sigsetjmp(jmpbuf, 1) != 0)
    {
        if(rtt_timeout(&rttinfo) < 0)
        {
            err_msg("dg_send_recv: no response from server, giving up");
            rttinit = 0;
            errno = ETIMEDOUT;
            return -1;
        }

        goto sendagain;
    }

    do
    {
        n = Recvmsg(fd, &msgrecv, 0);
    }while(n < sizeof(struct hdr) || recvhdr.seq != sendhdr.seq);

    alarm(0);

    rtt_stop(&rttinfo, rtt_ts(&rttinfo) - recvhdr.ts);

    return n - sizeof(struct hdr);
}

static void sig_alarm(int signo)
{
    siglongjmp(jmpbuf, 1);
}

ssize_t Dg_send_recv(int fd, const void *outbuff, size_t outbytes,
			 void *inbuff, size_t inbytes,
			 const sockaddr *destaddr, socklen_t destlen)
{
	ssize_t	n;

	n = dg_send_recv(fd, outbuff, outbytes, inbuff, inbytes,
					 destaddr, destlen);
	if (n < 0)
	{
        err_quit("dg_send_recv error");
    }

	return n;
}