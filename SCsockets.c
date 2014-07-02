//
//  SCsockets.c
//  events
//
//  Created by Leo Marcotte on 6/21/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <notify.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/event.h>
#include <unistd.h>
#include "SCevents.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


int SCgetevent_kqueueID();

int SCgetClientAddr(int socket,
                    char *return_ip,
                    int return_ip_length,
                    int *return_port,
                    char *hostName,
                    int hostNameLength);

int SCvalidate_event(EVENT_LIST *event);                              // Return non-zero if error

char *SCenviron(char *name);

int SCopen_event_listen(const char *host, int port);

int SCopen_event_socket(const char *host, int port);

int SCopen_event_path(const char *fullPath, char *ioCtl);

int SCget_eventPtr(EVENT_LIST **eventList,
                   int        *numberofEvents);

int SCcreate_event(char                 *name,                 // name of event
                   EVENT_TYPE             type,                 // type of event
                   char                  *path,                 // path (or host) to associate event with
                   int                    port,                 // port number if host provided
                   EVENT_IOCTL_TYPE      openType,              // ioCtl type to open file
                   char                  *function,             // function name
                   char                  *library,              // Library to search for function
                   int                   bufferLength,          // Length of buffer to allocate
                   char                  **buffer,              // returned address of allocated buffer
                   int                   *file_Descriptor);      // returned file descriptor


int SCopen_socket(int listenfd)
{
#include "SCevent_keywords.h"
    int            error_code;
    int            i;
    void           *handle;
    int            numberEvents;
    int            bufferLength = 0;
    EVENT_LIST     *event;
    EVENT_LIST     *workEvent;
    EVENT_LIST     *firstEmpty;
    EVENT_LIST     *nameFound;
   
    char            library[256];
    char            function[32];
    
    char            return_ip[64];
    int             return_ip_length = 64;
    int             return_port;
    char            hostName[64];
    int             hostNameLength = 64;
    
    struct sockaddr_in cliaddr;

    struct kevent   changelist;
    
    socklen_t clilen;
    int     connfd;
    
    clilen=sizeof(cliaddr);
    connfd = accept(listenfd,
                    (struct sockaddr *)
                    &cliaddr,&clilen);
    
    EV_SET(&changelist,
           connfd,
           kesocketFilter,
           kesocketFlag,
           kesocketFFlags,
           0,
           NULL);
    
    error_code = kevent(SCgetevent_kqueueID(),
                        &changelist,
                        1,
                        NULL,
                        0,
                        NULL);
    
    workEvent = NULL;
    firstEmpty = NULL;
    nameFound = NULL;
    
    SCgetClientAddr(connfd,
                     return_ip,
                     return_ip_length,
                    &return_port,
                     hostName,
                     hostNameLength);
    
    error_code = SCget_eventPtr(&event,
                         &numberEvents);
    
    if (error_code != 0)
    {
        fprintf(stdout,"%s: Error loading the event table. Terminating\n",__FUNCTION__);
        return(EXIT_FAILURE);
    }
    
    if (numberEvents > MAX_EVENTS)                                  // Precautionary to limit bounds
        numberEvents = MAX_EVENTS;
    
    workEvent = event;
    
                                                                    // Locate event record
    for(i=0;i < numberEvents;i++,workEvent++)                       // If exists, reuse, if deleted (-1 fd)
    {                                                               // Reset with new item
        if (workEvent->fd == 0)
            break;
        
        if (workEvent->fd < 0)
            firstEmpty = workEvent;
        
        if (strncmp(workEvent->name,hostName,strlen(workEvent->name)) == 0)
            nameFound = workEvent;
    }
    
    event =  workEvent;
    
    if (firstEmpty != NULL)
        event = firstEmpty;
    
    if (nameFound != NULL)
        event = nameFound;
    
    assert(event != NULL);
    
    bzero(event,sizeof(EVENT_LIST));
    
    event->fd = connfd;
    strncpy(event->name,hostName,sizeof(event->name));
    strncpy(event->path,hostName,sizeof(event->path));
    
    event->type = tcp_event;
    event->port = return_port;
    
    strncpy(event->ioCtl,ioCtlDesignations[0],sizeof(event->ioCtl));
    
    bufferLength = atoi(SCenviron("tcp_buffer"));
    
    if (bufferLength == 0)
        bufferLength = 256;
 
      event->buffer = malloc(bufferLength);
    
    if (SCvalidate_event(event) != EXIT_SUCCESS)                         /* Check fpor valid fields */
    {
        fprintf(stdout,
                "%s: Event data from config file is inaccurate for %s. Bypassing.\n",
                __FUNCTION__,
                event->name);
        
        event->fd = -1;                                                 /* delete entry             */
        
        return (EXIT_FAILURE);                                          /* and discontinue add      */
    }
    
    
    if (strlen(library) == 0)                                           /* Not specidied, use self  */
        handle = RTLD_SELF;
    else
    /* open the defined library  */
        handle = dlopen(library, RTLD_LOCAL | RTLD_LAZY);
    
    if (handle == NULL)
        fprintf(stdout,
                "%s: %s\n loading %s from %s\n",
                __FUNCTION__,
                dlerror(),
                function,
                library);
    
    event->fptr = dlsym(handle,function);
    
    if (event->fptr == NULL)
        fprintf(stdout,
                "%s: %s\n loading %s from %s\n",
                __FUNCTION__,
                dlerror(),
                function,
                library);
    
    if (handle != RTLD_SELF)
        dlclose(handle);
    
    
    return(EXIT_SUCCESS);

}

int SCread_socket(int fd,
                  char *message,
                  int *lth)
{
    socklen_t               len;
    struct sockaddr_in      *s;
    struct sockaddr_in6     *s6;
    struct sockaddr_storage addr;
    
    len = sizeof addr;
    
    getpeername(fd, (struct sockaddr*)&addr, &len);

    // deal with both IPv4 and IPv6:
    if (addr.ss_family == AF_INET)
        s = (struct sockaddr_in *)&addr;
    else  // AF_INET6
        s6 = (struct sockaddr_in6 *)&addr;
      
    *lth = (int) recvfrom(fd,message,*lth,0,(struct sockaddr *)s,&len);
    return(0);
}


int SCsend_socket(int fd, char *message, int lth)
{
    socklen_t               len;
    struct sockaddr_in      *s;
    struct sockaddr_in6     *s6;
    struct sockaddr_storage addr;
    
    len = sizeof addr;
    
    getpeername(fd, (struct sockaddr*)&addr, &len);
    
    // deal with both IPv4 and IPv6:
    if (addr.ss_family == AF_INET)
        s = (struct sockaddr_in *)&addr;
    else  // AF_INET6
        s6 = (struct sockaddr_in6 *)&addr;
    
    sendto(fd,message,lth,0,(struct sockaddr *)s,len);
    
    return(0);
    
}


