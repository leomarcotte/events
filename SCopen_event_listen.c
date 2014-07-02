//
//  SCopen_event_listen.c
//  events
//
//  Created by Leo Marcotte on 6/19/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <sys/event.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int SCopen_event_listen(const char *name, const char *host, int port)
{

struct addrinfo *addr;
struct addrinfo hints;
char   work[32];
int    error=0;
int    sckfd;

/* open a TCP socket */

bzero(&hints, sizeof(hints));
sprintf(work,"%d",port);

hints.ai_family = PF_UNSPEC;                /* any supported protocol */
hints.ai_flags = AI_PASSIVE;                /* result for bind() */
hints.ai_socktype = SOCK_STREAM;            /* TCP */
    
error = getaddrinfo (host, work, &hints, &addr);
    
if (error)
{
  fprintf(stdout,
         "%s (getaddrinfo): Error occurred opening %s on port %d\n%s\n",
            __FUNCTION__,
            host,
            port,
            gai_strerror(error));
    
    return(EXIT_FAILURE);
}

sckfd = socket(addr->ai_family,
               addr->ai_socktype,
               addr->ai_protocol);
    
if (sckfd < 0)
    {
        fprintf(stdout,
                "%s %s: (socket): Error occurred opening %s on port %d\n%s\n",
                __TIME__, __FUNCTION__,
                host,
                port,
                strerror(errno));
        
        return(-1);
    }

error = bind(sckfd,
             addr->ai_addr,
             addr->ai_addrlen);
    
if (error < 0)
    {
        fprintf(stdout,
                "%s %s: (bind): Error occurred opening %s on port %d\n%s\n",
                __TIME__, __FUNCTION__,
                host,
                port,
                strerror(errno));
        
        return(-2);
    }

    
error = listen(sckfd, 5);
    
if (error < 0)
    {
        fprintf(stdout,
                "%s %s: (listen): Error occurred opening %s on port %d\n%s\n",
                __TIME__, __FUNCTION__,
                host,
                port,
                strerror(errno));
        
        return(-3);
    }

fprintf(stdout,
        "%s Comm %s Listening[%d]  %s on port %d.\n",
        __TIME__,
        name,
        sckfd,
        host,
        port);
return(sckfd);

}