
#ifndef WRAP_SOCKET_H
#define WRAP_SOCKET_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERV_PORT ((uint16_t)9877)
#define LISTENQ 5

typedef struct sockaddr sockaddr;

int Socket(int, int, int);

void Bind(int, const sockaddr *, socklen_t);

void Connect(int, const sockaddr *, socklen_t);

void Listen(int, int);

int	Accept(int, sockaddr *, socklen_t *);

void Inet_pton(int, const char *, void *);

const char *Inet_ntop(int, const void *, char *, size_t);

int Sockfd_to_family(int sockfd);

int Getpeername(int, sockaddr *, socklen_t *);
int Getsockname(int, sockaddr *, socklen_t *);

void Shutdown(int, int);

#endif
