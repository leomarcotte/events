//
//  SCopen_event_socket.c
//  events
//
//  Created by Leo Marcotte on 6/18/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <sys/event.h>
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int SCopen_event_socket(const char *host, int port)
{
    struct hostent     *hp;
    struct sockaddr_in server;
    int sckfd;
    
    assert(host != NULL);
    
    if ((hp = gethostbyname(host)) == NULL)
       {
        fprintf(stdout,
                "%s %s (gethostbyname): Error occurred opening %s on port %d\n%s\n",
                __TIME__,
                __FUNCTION__,
                host,
                port,
                strerror(errno));
           
        return(-1);
       }
    
    if ((sckfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stdout,
                "%s %s (socket): Error occurred opening %s on port %d\n%s\n",
                __TIME__, __FUNCTION__,
                host,
                port,
                strerror(errno));
        
        return(-2);
    }
    
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr = *((struct in_addr *)hp->h_addr);
    memset(&(server.sin_zero), 0, 8);
    
    if (connect(sckfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0)
    {
        fprintf(stdout,
                "%s %s (connect): Error occurred opening %s on port %d\n%s\n",
                __TIME__, __FUNCTION__,
                host,
                port,
                strerror(errno));
        
        return(-3);
    }
    
    return(sckfd);
}

