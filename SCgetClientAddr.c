//
//  SCgetClientAddr.c
//  events
//
//  Created by Leo Marcotte on 6/22/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <netdb.h>

int SCgetClientAddr(int socket,
                  char *return_ip,
                  int return_ip_length,
                  int *return_port,
                  char *hostName,
                  int hostNameLength)
{
    // assume s is a connected socket
    
    socklen_t               len;
    struct in_addr          ip;
    struct hostent          *hp;
    struct sockaddr_storage addr;
    char ipstr[INET6_ADDRSTRLEN];
    int port = 0;
    
    len = sizeof addr;
    getpeername(socket, (struct sockaddr*)&addr, &len);
    
    // deal with both IPv4 and IPv6:
    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
    } else { // AF_INET6
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        port = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
    }
    
    inet_aton(ipstr, &ip);
    
    hp = gethostbyaddr((const void *)&ip,sizeof ip, addr.ss_family);
    
    if (hp != NULL)
        strncpy(hostName, hp->h_name,hostNameLength);
    else
        strncpy(hostName,ipstr,hostNameLength);
    
    strncpy(return_ip,ipstr,return_ip_length);
    
    *return_port = port;

    
    return(0);
}
