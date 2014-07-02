//
//      SCinitialize_events.c
//  events
//
//  Created by Leo Marcotte on 6/16/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//
//  This routines accepts a internal event table that is populated with the
//  paths and types of events to be established.
//  The routine creates a Kqueue which the kFD is passed back to the caller.
//  The kQueue is loaded with each of the evetns provided in the eventList
//  up to the count of how many their are.
//
//  The kQueue chnagelist is returned back to the user populated with
//  all queues set.
//
//   1) calls kQueue to construct queue,
//   2) Loop through the eventList provided for the number of events indicated.
//   3) Call EV_SET for each event, constructing the changeList for the kQueue established.
//   4) Calls kevent with the chnagelist to establish setup the list of events.
//   5) If time is not null, sets the timeer for the event.


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/event.h>
#include <unistd.h>
#include <dlfcn.h>
#include "SCevents.h"

char *SCenviron(char *name);

void *SCload_event_function(char *name);

int SCload_event_config(char reloadFlag);                      // Set to 'R' to reload

int SCcreate_event(int                   listeningFD,          // used for accepting connections
                  char                  *name,                 // name of event
                  EVENT_TYPE             type,                 // type of event
                  char                  *path,                 // path (or host) to associate event with
                  int                    port,                 // port number if host provided
                  char                  *ioCtl,                // ioCtl type to open file
                  char                  *processfunction,      // processing function name
                  char                  *closefunction,        // processing function on close
                  char                  *errorfunction,        // error procesing function
                  char                  *library,              // Library to search for function
                  char                  socketType,            // 'L' for listen
                  int                   bufferLength,          // Length of buffer to allocate
                  char                  **buffer,              // returned address of allocated buffer
                  int                   *file_Descriptor);    // returned file descriptor

void SCinitEventAddressList();

int SCdel_bufferPtr(char **buffer);

int SCdel_eventPtr(char **event);

int SCevent_kqueue;


int SCgetevent_kqueueID()
{
    return(SCevent_kqueue);
}

int SCinitialize_events()
{
#include "SCevent_keywords.h"
    
    EVENT_LIST    evt;
    EVENT_LIST   *eventList = &evt;
    int           kQueueID;
    struct kevent changelist;
    char          *ptr = NULL;
    int           i, x, ret      = 0;
    char          work[256];
    char          library[256];
    char          processfunction[64];
    char          closefunction[64];
    char          errorfunction[64];
    char          nameWork[256];
    int           numberEvents;
    void         *handle;
  
    SCinitEventAddressList();                           // Initialize event pointers
    
    bzero(eventList,sizeof(EVENT_LIST));
    bzero(library,sizeof(library));
    bzero(work,sizeof(work));
    bzero(processfunction,sizeof(processfunction));
    bzero(closefunction,sizeof(closefunction));
    bzero(errorfunction,sizeof(errorfunction));
    bzero(nameWork,sizeof(nameWork));
                                                        // Load system configuration
    SCload_event_config(' ');
    
                                                        // Load Shared Memory
    
    handle = dlopen("/usr/local/lib/libsharedmem.dylib", RTLD_LAZY | RTLD_LOCAL);
    if (handle == NULL)
       fprintf(stdout,"%s %s: Shared memory not implemented in the process(L).\n",
            __TIME__, __FUNCTION__);
    else
       {
           int (*svminitptr)();
           svminitptr = dlsym(handle,"SCsharedmem_init");
           if (svminitptr == NULL)
               fprintf(stdout,"%s %s: Shared memory not implemented in the process(F).\n",
                       __TIME__, __FUNCTION__);
           else
               (svminitptr)();
       }

                                                        // Initialize the change list buffer
   for (i=0;i < numberEvents;i++)
        memset(&changelist,'\0',sizeof(struct kevent));
    
                                                        // create a new kernel event queue
  if ((kQueueID = kqueue()) < 0)
        {
            fprintf(stdout,"%s %s: Unable to establish queue system. %s. Aborting.",
                    __TIME__, __FUNCTION__,
                    strerror(errno));
            exit(-1);
        }
  
    SCevent_kqueue = kQueueID;
    
    strncpy(work,SCenviron("standard_process_events"),sizeof(work));
    
    if (strlen(work) == 0)
       {
           fprintf(stdout,
                   "%s %s :No standard_process_events defined in configuration\n",
                  __TIME__, __FUNCTION__);
        return(EXIT_SUCCESS);
       }
    
    ptr = strtok(work,",\0\n");
    
    if (ptr == NULL)
    {
        fprintf(stdout,
                "%s %s:Error processing standard_process_events defined in configuration\n",
                __TIME__,__FUNCTION__);
        return(EXIT_FAILURE);
    }
    
    
  
    strncpy(eventList->name,                            // Populate with name
            ptr,
            sizeof(eventList->name));
    
                                                        // Build change list
       while(strlen(eventList->name) > 0)
       {
           eventList->fptr = SCload_event_function(eventList->name);
           
           if (eventList->fptr == NULL)
               fprintf(stdout,"%s: Function not loaded for event %s\n",
                       __FUNCTION__,
                       eventList->name);
           
           getcwd(eventList->current_dir, sizeof(eventList->current_dir));
           
           eventList->last_use = 0;
           
           eventList->bufferLth = 0;
           sprintf(nameWork,"%s_buffer",eventList->name);
           eventList->bufferLth = atoi(SCenviron(nameWork));
           
           if (eventList->bufferLth <= 0)                           // Default to 256
               eventList->bufferLth = 256;
           
           sprintf(nameWork,"%s_path",eventList->name);
           strncpy(eventList->path,SCenviron(nameWork),sizeof(eventList->path));
           
                                                                        // Note: load the value from config
           sprintf(nameWork,"%s_ioctl",eventList->name);
           strncpy(eventList->ioCtl,SCenviron(nameWork),sizeof(eventList->ioCtl));
           
           for(x=0;strlen(ioTypes[x]) > 0;x++)                          // if a keyword was used, replace it
               if(strncmp(ioTypes[x],eventList->ioCtl,strlen(eventList->ioCtl)) == 0)
                 {
                  strncpy(eventList->ioCtl,ioCtlDesignations[x],sizeof(eventList->ioCtl));
                  break;
                 }
           sprintf(nameWork,"%s_port",eventList->name);
           eventList->port = atoi(SCenviron(nameWork));
           
           sprintf(nameWork,"%s_type",eventList->name);
           strcpy(work,SCenviron(nameWork));
           
           eventList->type = 0;
           for(x=0;strlen(eTypes[x]) > 0;x++)
               if (strncmp(eTypes[x],work,strlen(work)) == 0)
                  {
                   eventList->type = x;
                   break;
                  }
           
           sprintf(nameWork,"%s_library",eventList->name);
           strncpy(library,SCenviron(nameWork),sizeof(library));
           
           sprintf(nameWork,"%s_pfunction",eventList->name);
           strncpy(processfunction,SCenviron(nameWork),sizeof(processfunction));
 
           sprintf(nameWork,"%s_cfunction",eventList->name);
           strncpy(closefunction,SCenviron(nameWork),sizeof(closefunction));
           
           sprintf(nameWork,"%s_efunction",eventList->name);
           strncpy(errorfunction,SCenviron(nameWork),sizeof(errorfunction));

           if (strlen(processfunction) == 0)
               strcpy(processfunction,DEFAULT_PROC_FUNC);
           
           if (strlen(closefunction) == 0)
               strcpy(closefunction,DEFAULT_CLOSE_FUNC);
           
           if (strlen(errorfunction) == 0)
               strcpy(errorfunction,DEFAULT_ERROR_FUNC);

           ret = SCcreate_event(eventList->fd,                   // used for accepting connections
                                eventList->name,                 // name of event
                                eventList->type,                 // type of event
                                eventList->path,                 // path (or host) to associate event with
                                eventList->port,                 // port number if host provided
                                eventList->ioCtl,                // ioCtl type to open file
                                processfunction,                 // Processing function name
                                closefunction,                   // closing function name
                                errorfunction,                   // error function 
                                library,                         // Library to search for function
                                'L',                             // 'L' Listen sockets, ignored on files
                                eventList->bufferLth,            // Length of buffer to allocate
                               &eventList->buffer,               // returned address of allocated buffer
                               &eventList->fd);                  // returned file descriptor
           
           if (ret != 0)
              {
               fprintf(stdout,"%s %s: ERROR creating event %s. %s Bypassing\n",
                       __TIME__, __FUNCTION__,
                       eventList->name,
                       strerror(errno));
                                                                // Note, drop thru to get next service to start
                // after removing this event
              }
  
           ptr = strtok(NULL,",\0\n");
           
           if (ptr == NULL)
               break;
           
          bzero(eventList,sizeof(EVENT_LIST));                // clear the new event area
           
          strncpy(eventList->name,                            // copy the name into the work area
                       ptr,
                       sizeof(eventList->name));
           
       } // while loop

/* Close out */
    
return(kQueueID);
}
