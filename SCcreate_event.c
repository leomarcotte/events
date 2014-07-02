//
//  SCcreate_event->c
//  events
//
//  Created by Leo Marcotte on 6/20/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

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

int SCopen_event_accept(int listenfd,
                        int  *rport,
                        char *host,
                        int  hostlth,
                        char *ipAddress,
                        int  ipAddressLth);


int SCget_bufferPtr(char ** buffer, int bufferLength);

int SCdel_bufferPtr(char **buffer);

int SCvalidate_event(EVENT_LIST *event);                              // Return non-zero if error

int SCdel_eventPtr(EVENT_LIST **event);

char *SCenviron(char *name);

int SCopen_event_listen(const char *name, const char *host, int port);

int SCopen_event_socket(const char *host, int port);

int SCopen_event_path(const char *name, const char *fullPath, char *ioCtl);

int SCget_eventPtr(EVENT_LIST **eventList);

int SCgetevent_kqueueID();

int SCcreate_event(int                  listeninfFD,           // Used for accept calls
                  char                  *name,                 // name of event
                  EVENT_TYPE             type,                 // type of event
                  char                  *path,                 // path (or host) to associate event with
                  int                    port,                 // port number if host provided
                  char                  *ioCtl,                // ioCtl type to open file
                  char                  *processfunction,      // Processing function name
                  char                  *closefunction,        // closing function name
                  char                  *errorfunction,
                  char                  *library,              // Library to search for function
                  char                  socketType,            // 'L' for listen
                  int                   bufferLength,          // Length of buffer to allocate
                  char                  **buffer,              // returned address of allocated buffer
                  int                   *file_Descriptor)      // returned file descriptor


{
  
#include "SCevent_keywords.h"
    
    int             switchType     = 0;
    void           *handle;
    int            ret;
    EVENT_LIST     *event;
    struct  kevent changelist;

    
    if (strncmp(SCenviron("debug"),"debug",5) == 0)
       {
         fprintf(stdout,"%s%s: %s[%u] - %s [%u]\n%s\t%s\t%s\n",
                 __TIME__,
                 __FUNCTION__,
                 name,
                 type,
                 path,
                 port,
                 processfunction,
                 closefunction,
                 library);
       }
    
                                                    // Get a new event slot
    ret = SCget_eventPtr(&event);
    
    if (ret != 0)
    {
        fprintf(stdout,"%s %s: Error loading the event table. Terminating\n",__TIME__,__FUNCTION__);
        return(EXIT_FAILURE);
    }
    
    assert(event != NULL);
    
    strncpy(event->name,name,sizeof(event->name));
    strncpy(event->path,path,sizeof(event->path));
    
    event->type = type;
    event->port = port;
    
    if (type == file_event  ||
        type == queue_event ||
        type == config_event)
        strncpy(event->ioCtl,ioCtl,sizeof(event->ioCtl));
    
    if (bufferLength <= 0)
        bufferLength = 256;                                             // Default length to 256 fpr buffer
        
    ret = SCget_bufferPtr(&event->buffer,bufferLength);

    if (ret != 0)
    {
        fprintf(stdout,"%s %s: No message buffer provided for %s, Continuing\n",
                __TIME__,__FUNCTION__, event->name);
        event->buffer = NULL;
    }
    
    event->bufferLth = bufferLength;
                                                                        /* If library is            */
    if (strlen(library) == 0)                                           /* Not specidied, use self  */
        handle = RTLD_SELF;
    else
                                                                        /* open the defined library  */
        handle = dlopen(library, RTLD_LOCAL | RTLD_LAZY);
    
                                                                        // Set processing routines
    event->fptr = NULL;                                                 // note, process, error, and close
    event->eptr = NULL;                                                 // routines are defined in the config
    event->cptr = NULL;                                                 // using keywords:
                                                                        //  <service_type>_pfunction - process
    if (strlen(processfunction) == 0)                                    //  <service_type_efunction  - error
        strncpy(processfunction,DEFAULT_PROC_FUNC,32);                  //  <service_type>_cfunction = closing
                                                                        //  usually set in SCstart_event_process
    if (strlen(errorfunction) == 0)                                      // andcalled by SCwait_event based on
        strncpy(errorfunction,DEFAULT_ERROR_FUNC,32);                 // event state.
    
    if (strlen(closefunction) == 0)
        strncpy(closefunction,DEFAULT_CLOSE_FUNC,32);
    
    event->fptr = dlsym(handle,processfunction);
    
    event->eptr = dlsym(handle,errorfunction);

    event->cptr = dlsym(handle,closefunction);
    
    if (event->fptr == NULL)
        event->fptr = dlsym(RTLD_SELF,processfunction);
    
    if (event->eptr == NULL)
        event->eptr = dlsym(RTLD_SELF,errorfunction);
    
    if (event->cptr == NULL)
        event->cptr = dlsym(RTLD_SELF,closefunction);
    
    
    if (handle != RTLD_SELF)
        dlclose(handle);
    
    event->status = opened;
    switchType = event->type;
    
    switch(switchType)
    {
        case eventNotdefined:                                    // Don't know what we have
            if (event->buffer != NULL)
                SCdel_bufferPtr(&event->buffer);
            
            SCdel_bufferPtr((char **) &event);
            return(EXIT_FAILURE);
            
        case file_event:                                       // FILE
        case queue_event:
        case config_event:
            
            event->fd = SCopen_event_path(event->name,
                                          event->path,
                                          event->ioCtl);
            keFlag   = keFileFlag;
            keFilter = kefileFilter;
            keFFlags = kefileFFlags;
            break;
            
        case websocket_event:                              // SOCKET
        case tcp_event:
            
            
            switch (socketType)
                   {
                       case 'L':                              // LISTEN SOCKET
                           
                           event->fd =  SCopen_event_listen(event->name,
                                                            event->path,
                                                            event->port);
                           if (event->fd > 0)
                               event->status = listening;
                           break;
                       case 'A':                              // ACCEPT SOCKET


                           event->fd =  SCopen_event_accept(listeninfFD,
                                                            &event->port,
                                                            event->path,
                                                            sizeof(event->path),
                                                            event->ipAddress,
                                                            sizeof(event->ipAddress));
                           if (event->fd > 0)
                               event->status = connected;
                           
                           break;
                
                       default:                               // CLIENT SOCKET
                
                           event->fd =  SCopen_event_socket(event->path,
                                                            event->port);
                break;
        }
            
            keFlag   = kesocketFlag;
            keFilter = kesocketFilter;
            keFFlags = kesocketFFlags;
            break;
    }
    
    *file_Descriptor = event->fd;
    
    if (SCvalidate_event(event) != EXIT_SUCCESS)
       {
        fprintf(stdout,
                "%s %s: Event data from config file is inaccurate for %s. Bypassing.\n",
                __TIME__,
                __FUNCTION__,
                event->name);
        
         close(event->fd);                                   // Close the file
         SCdel_bufferPtr((char**) &event);                   // Clear event allocation
        return(EXIT_FAILURE);
        
       }
    
    EV_SET(&changelist,
           event->fd,
           keFilter,
           keFlag,
           keFFlags,
           0,
           event);
    
    ret = kevent(SCgetevent_kqueueID(),
                &changelist,
                 1,
                 NULL,
                 0,
                 NULL);

    if (ret < 0)
        fprintf(stdout,"%s %s: Error loading event %s into queue. %s\n",
                __TIME__,__FUNCTION__,event->name,strerror(errno));
    
    return(EXIT_SUCCESS);
}

