#ifndef SERVER_H
#define SERVER_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "msghandle.h"

#define IPADDRESS INADDR_ANY
#define PORT 3456
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

#endif