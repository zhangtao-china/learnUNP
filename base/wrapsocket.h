
#ifndef WRAP_SOCKET_H
#define WRAP_SOCKET_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERV_PORT 9877
#define LISTENQ 5

typedef struct sockaddr sockaddr;

int Socket(int, int, int);

void Bind(int, const sockaddr *, socklen_t);

void Connect(int, const sockaddr *, socklen_t);

void Listen(int, int);

int	Accept(int, sockaddr *, socklen_t *);

void Inet_pton(int, const char *, void *);

#endif
