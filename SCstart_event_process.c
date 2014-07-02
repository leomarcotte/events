//
//  SCstart_event_process.c
//  events
//
//  Created by Leo Marcotte on 6/19/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "SCevents.h"
#include "config.h"

char *SCenviron(char * name);

int *SCload_event_function(char * indexName);

int SCevent_process_return(char *buffer, int bufferLength);

int SCcreate_event(int                   listeningFD,          // used for accepting connections
                  char                  *name,                 // name of event
                  EVENT_TYPE             type,                 // type of event
                  char                  *path,                 // path (or host) to associate event with
                  int                    port,                 // port number if host provided
                  char                  *ioCtl,                // ioCtl type to open file
                  char                  *processfunction,      // processing function name
                  char                  *closefunction,        // processing function on close
                  char                  *errorfunction,        // error procedure to call on processing errors
                  char                  *library,              // Library to search for function
                  char                  socketType,            // 'L' for listen
                  int                   bufferLength,          // Length of buffer to allocate
                  char                  **buffer,              // returned address of allocated buffer
                  
                  int                   *file_Descriptor);    // returned file descriptor


int SCstart_event_process(EVENT_LIST *event)
{
    
#include "SCevent_keywords.h"
    
    int                 bufferLength;
    int                 error_code;
    int                 newClientSocket;
    char                nameWork[256];
    char                processfunction[32];
    char                closefunction[32];
    char                errorfunction[32];
    char                library[256];
    
                                                            // get config based on service type
    
    strncpy(nameWork,eTypes[event->type],sizeof(nameWork));
    strncat(nameWork,"_pfunction",sizeof(nameWork) - strlen(nameWork) - 1);
    strncpy(processfunction,SCenviron(nameWork),sizeof(processfunction));
    
    strncpy(nameWork,eTypes[event->type],sizeof(nameWork));
    strncat(nameWork,"_cfunction",sizeof(nameWork) - strlen(nameWork) - 1);
    strncpy(closefunction,SCenviron(nameWork),sizeof(closefunction));
    
    strncpy(nameWork,eTypes[event->type],sizeof(nameWork));
    strncat(nameWork,"_efunction",sizeof(nameWork) - strlen(nameWork) - 1);
    strncpy(errorfunction,SCenviron(nameWork),sizeof(errorfunction));
    
    strncpy(nameWork,eTypes[event->type],sizeof(nameWork));
    strncat(nameWork,"_library",sizeof(nameWork) - strlen(nameWork) - 1);
    strncpy(library,SCenviron(nameWork),sizeof(library));
    
    if (strlen(processfunction) == 0)
        strcpy(processfunction,DEFAULT_PROC_FUNC);
    
    if (strlen(closefunction) == 0)
        strcpy(closefunction,DEFAULT_CLOSE_FUNC);
    
    if (strlen(errorfunction) == 0)
        strcpy(errorfunction,DEFAULT_ERROR_FUNC);
    
    strncpy(nameWork,eTypes[event->type],sizeof(nameWork));
    strncat(nameWork,"_buffer",sizeof(nameWork) - strlen(nameWork) - 1);
    bufferLength = atoi(SCenviron(nameWork));
    
    if (bufferLength == 0)                                  // default Buffer Length 256
        bufferLength = 256;
    
    error_code = 0;
                        
                                                            // Creat a new event for the started.
                                                            // the old one is the listen one that
                                                            // stays in tact.

    error_code = SCcreate_event(
                         event->fd,                         // used to accept connection from remote
                         event->name,                       // name of event
                         event->type,                       // type of event
                         event->path,                       // path (or host) to associate event with
                         event->port,                       // port number if host provided
                         event->ioCtl,                      // ioCtl type to open file
                         processfunction,                   // Processing function name
                         closefunction,                     // closing function name
                         errorfunction,
                         library,                           // Library to search for function
                         'A',                               // 'L' Listen sockets, ignored on files
                         bufferLength,                      // Length of buffer to allocate
                         &event->buffer,                    // returned address of allocated buffer
                         &newClientSocket);                 // returned file descriptor
    
 
    
    return(EXIT_SUCCESS);
}
