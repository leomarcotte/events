//
//  SCopen_event_accept.c
//  events
//
//  Created by Leo Marcotte on 6/24/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <sys/event.h>
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int SCgetClientAddr(int socket,
                    char *return_ip,
                    int   return_ip_length,
                    int  *return_port,
                    char *hostName,
                    int   hostNameLength);

int SCopen_event_accept(int listenfd,
                        int  *rport,
                        char *host,
                        int  hostlth,
                        char *ipAddress,
                        int  ipAddressLth)
{
    int                 sckfd;
    uint                size;
    long on = 1L;
    
    struct sockaddr_in  clientname;


    bzero(host,hostlth);
    
    sckfd = accept     (listenfd,(struct sockaddr *) &clientname,&size);
    
    if (sckfd < 0)
    {
        fprintf(stdout,"%s %s: Unable to accept connection. %s\n",__TIME__,__FUNCTION__,strerror(errno));
    }
    
                                                        // Set non blocking on socket
    ioctl(sckfd, (int)FIONBIO, (char *)&on);
    
    SCgetClientAddr(sckfd,
                    
                    ipAddress,
                    ipAddressLth,
                    rport,
                    host,
                    hostlth);


    fprintf(stdout,"%s CONNECT %s[%d] established.\n",__TIMESTAMP__,host,*rport);
    return(sckfd);
}


