//
//  SCevent_process_return.c
//  events
//
//  Created by Leo Marcotte on 6/22/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>

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

char *SCenviron(char *name);

void *SCload_event_function(char *name);

int SCcreate_event(int                   listeningFD,           // used for accepting connections
                   char                  *name,                 // name of event
                   EVENT_TYPE             type,                 // type of event
                   char                  *path,                 // path (or host) to associate event with
                   int                    port,                 // port number if host provided
                   char                  *ioCtl,                // ioCtl type to open file
                   char                  *processfunction,      // processing function name
                   char                  *closefunction,        // processing function on close
                   char                  *errorfunction,        // error processing
                   char                  *library,              // Library to search for function
                   char                  socketType,            // 'L' for listen
                   int                   bufferLength,          // Length of buffer to allocate
                   char                  **buffer,              // returned address of allocated buffer
 
                   int                   *file_Descriptor);    // returned file descriptor

int SCdelete_event(EVENT_LIST * event, int fd);

int SCevent_process_return(char *message, int msgLth)
{
    
#include "SCevent_keywords.h"
    
#define MAX_FLDS 30
    
    EVENT_LIST       evt;
    EVENT_LIST      *event = &evt;
    char            work[1028];
    char            nameWork[256];
    char            library[256];
    char            processfunction[64];
    char            closefunction[64];
    char           *fields[MAX_FLDS];
    int             numFields;
    int             i;
    int             ret;
    char           *cmd;
    char           *fd;
    char           *host;
    char           *port;
    char           *pgm;
    char           *ioCtl;
    char           *tokr;
    char           *dummy[]    = {"not defined"};
    char           *commands[] = {"EVTSREGISTER","EVTCLOSE",""};

    /*
     *** Initialization Section
     */
    
    
    bzero(work,sizeof(work));
    bzero(nameWork,sizeof(nameWork));
    bzero(event,sizeof(EVENT_LIST));
    bzero(fields,sizeof(fields));
    bzero(library,sizeof(library));
    bzero(processfunction,sizeof(processfunction));
    bzero(closefunction, sizeof(closefunction));
    
    strncpy(work,message,sizeof(work));

    /*
     *** Input Section
     */
    
    fields[0] = strtok_r(work," ,\n",&tokr);
    if (fields[0] == NULL)
       {
           fprintf(stdout,
           "%s: Invalid Message structure, ignoring\n",
           __FUNCTION__);
           
           return(EXIT_FAILURE);
       }
    
                                                            // If message header does not indicate SCRETURN
    if (strncmp(fields[0],"SCRETURN",8) != 0)               // do not process
       {
           fprintf(stdout,
                   "%s: Message does not contain SCRETURN, ignoring\n%s\n",
                   __FUNCTION__,
                   message);
           
           return(EXIT_FAILURE);
       }
                                                           // Loop through the fields separating by comma
    for(numFields=0;numFields < MAX_FLDS;numFields++)
       {
        fields[numFields] = strtok_r(NULL,",\n",&tokr);
        if (fields[numFields] == NULL)
             break;
       }
    
    host = NULL;
    port = NULL;
    fd   = NULL;
    cmd  = NULL;
    pgm  = NULL;
    ioCtl= (char*) dummy;
                                                          // Break components from message
    
    for(i=0;i < numFields;i++)
       {
           if (memcmp(fields[i],"HOST",4) == 0)
                host = fields[i] + 5;
           
           if (memcmp(fields[i],"PATH",4) == 0)
               host = fields[i] + 5;
           
           if (memcmp(fields[i],"PORT",4) == 0)
                port = fields[i] + 5;
           
           if (memcmp(fields[i],"FD",2) == 0)
                fd= fields[i] + 3;
                       
          if (memcmp(fields[i],"CMD",3) == 0)
                cmd= fields[i] + 4;
           
           if (memcmp(fields[i],"PGM",3) == 0)
                pgm= fields[i] + 4;
           
           if (memcmp(fields[i],"IOCTL",5) == 0)
               ioCtl= fields[i] + 6;
       }
 
    /*
     *** Selection Section
     */
    
   if (host == NULL ||
       port == NULL ||
       fd   == NULL ||
       pgm  == NULL ||
       cmd  == NULL)
      {
        fprintf(stdout,
                "%s: SCRESPONSE incorrect data\n%s\n",
                __FUNCTION__,
                message);
          
          return(EXIT_SUCCESS);
      }
    
    /*
     *** Processing Section
     */
    
    for(i=0;strlen(commands[i]) > 0;i++)
        if (strncmp(commands[i],cmd,strlen(commands[i])) == 0)
            break;
    
    switch(i)
    {
        case 0:                                                      // EVTSREGISTER COMMAND
                event->fd = atoi(fd);
    
                strncpy(event->name,pgm,sizeof(event->name));
    
                event->fptr = SCload_event_function(event->name);
                getcwd(event->current_dir, sizeof(event->current_dir));
    
                event->last_use = 0;
    
                event->bufferLth = 0;
                sprintf(nameWork,"%s_buffer",event->name);
                event->bufferLth = atoi(SCenviron(nameWork));
    
                if (event->bufferLth <= 0)                           // Default to 256
                    event->bufferLth = 256;
    
                strncpy(event->path,host,sizeof(event->path));
    
                strncpy(event->ioCtl,ioCtl,sizeof(event->ioCtl));
    
                for(i=0;strlen(ioTypes[i]) > 0;i++)
                    if(strncmp(ioTypes[i],event->ioCtl,strlen(event->ioCtl)) == 0)
                      {
                       strncpy(event->ioCtl,ioCtlDesignations[i],sizeof(event->ioCtl));
                       break;
                      }

                event->port = atoi(port);
    
                sprintf(nameWork,"%s_type",event->name);
                strcpy(work,SCenviron(nameWork));
    
                event->type = 0;
                for(i=0;strlen(eTypes[i]) > 0;i++)
                    if (strncmp(eTypes[i],work,strlen(work)) == 0)
                       {
                        event->type = i;
                        break;
                       }
    
                sprintf(nameWork,"%s_library",event->name);
                strncpy(library,SCenviron(nameWork),sizeof(library));
    
                sprintf(nameWork,"%s_procedure",event->name);
                strncpy(processfunction,SCenviron(nameWork),sizeof(processfunction));
            
                if (strlen(processfunction) == 0)                                // Use the pgm= provided in command
                    strncpy(processfunction,pgm,sizeof(processfunction));       // if it was not found in config
    
                sprintf(nameWork,"%s_closingprocedure",event->name);
                strncpy(closefunction,SCenviron(nameWork),sizeof(closefunction));
    
    
                for(i=0;strlen(commands[i]) > 0;i++)
                    if (strncmp(cmd,commands[i],strlen(commands[i])) == 0)
                        break;
            
                ret = SCcreate_event(event->fd,
                                     event->name,                 // name of event
                                     event->type,                 // type of event
                                     event->path,                 // path (or host) to associate event with
                                     event->port,                 // port number if host provided
                                     event->ioCtl,                // ioCtl type to open file
                                     processfunction,             // Processing function name
                                     errorfunction,               // error processing function
                                     closefunction,               // closing function name
                                     library,                     // Library to search for function
                                     'A',                         // 'L' Listen sockets, ignored on files
                                     event->bufferLth,            // Length of buffer to allocate
                                     &event->buffer,              // returned address of allocated buffer
                                     &event->fd);                 // returned file descriptor
                
                  break;
            case 1:                                                           // EVTCLOSE COMMAND
            SCdelete_event(event, event->fd);
                break;
            default:
                fprintf(stdout,"%s: Unknown transaction request [%s]\n",
                        __FUNCTION__,
                        cmd);
                break;
        }
    
    /*
     *** Closure Section
     */
    return(0);
    
}


