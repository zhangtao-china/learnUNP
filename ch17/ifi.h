#ifndef IFI_H
#define IFI_H

#include <net/if.h>
#include "../base/wrapsocket.h"

#define IFI_NAME  16
#define IFI_HADDR  8

struct ifi_info
{
    char ifi_name[IFI_NAME];
    short ifi_index;
    short ifi_mtu;
    u_char ifi_haddr[IFI_HADDR];
    u_short ifi_hlen;
    short ifi_flags;
    short ifi_myflags;
    struct sockaddr *ifi_addr;
    struct sockaddr *ifi_brdaddr;
    struct sockaddr *ifi_dstaddr;
    struct ifi_info *ifi_next;
};

#define IFI_ALIAS  1

struct ifi_info *get_ifi_info(int, int);
struct ifi_info *Get_ifi_info(int, int);
void free_ifi_info(struct ifi_info *);

#endif