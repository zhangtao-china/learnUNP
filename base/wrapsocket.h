
#ifndef WRAP_SOCKET_H
#define WRAP_SOCKET_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "commondef.h"


typedef struct sockaddr sockaddr;

int Socket(int, int, int);

void Bind(int, const sockaddr *, socklen_t);

void Connect(int, const sockaddr *, socklen_t);

void Listen(int, int);

int	Accept(int, sockaddr *, socklen_t *);

void Inet_pton(int, const char *, void *);

const char *Inet_ntop(int, const void *, char *, size_t);

char * sock_ntop(const sockaddr *, socklen_t, char *, size_t);

char * sock_ntop_host(const struct sockaddr *, socklen_t);

int Sockfd_to_family(int sockfd);

int Getpeername(int, sockaddr *, socklen_t *);
int Getsockname(int, sockaddr *, socklen_t *);

void Getsockopt(int, int, int, void *, socklen_t *);

void Setsockopt(int, int, int, const void *, socklen_t);

void Shutdown(int, int);

int tcp_connect(const char *host, const char * serv);

struct addrinfo * host_serv(const char *host, const char *serv, int family, int socktype);

#endif
