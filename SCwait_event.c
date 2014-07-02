//
//  SCwait_event.c
//  events
//
//  Created by Leo Marcotte on 6/15/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/event.h>
#include <unistd.h>
#include "SCevents.h"
#include "libevent.h"

char * SCenviron(char * name);

int SCdelete_event(int kqFD,
                   EVENT_LIST        *event,
                   int                fd,
                   void              *message);

void SCdisplayEvent(struct kevent    *event,
                    int nevents);

int SCstart_event_process(EVENT_LIST *event);

int SCget_eventPtr(EVENT_LIST       **eventList,
                   int               *numberofEvents);

int SCgetevent_kqueueID();

#define ELIST_MAX    25

EVENT_LIST * SCwait_event(int     kQueueID)


{
    int              return_code = 0;
    int              error_code  = 0;
    int              i;
    int              elistNumber = 0;
    int              call_length;
    int              filedes;
    EVENT_STRUCTURE  call_message;

    struct kevent    eventList[ELIST_MAX];
    EVENT_LIST      *event;

#include "SCevent_keywords.h"
    
    kQueueID = SCgetevent_kqueueID();

    while(1)
    {
        fflush(stdout);
        bzero(&call_message,sizeof(EVENT_STRUCTURE));
        bzero(eventList,sizeof(eventList));
        return_code = 0;
        elistNumber = ELIST_MAX;
        
        return_code = kevent(kQueueID,
                            NULL,
                            0,
                            eventList,
                            elistNumber,
                            NULL);
    
       if (return_code < 0)
          {
              
           if (errno == EINTR)                          // EINTR Interrupted system call
               continue;
              
           fprintf(stderr,
                   "%s: %s\n",
                   __FUNCTION__,
                   strerror(errno));
            return(NULL);
          }
    
       if (return_code == 0)
           continue;
        
       for(i=0; i < return_code;i++)
           {
            event = eventList[i].udata;
/*
 * Event error has occurred, report it.
 */
            if (eventList[i].flags & EV_ERROR)
                fprintf(stderr,
                        "EV_ERROR: %s\n",
                        strerror((int) eventList[i].data));
/*
 * EOF/connection closed detected
 */
            if (eventList[i].flags & EV_EOF)            //  then close it.
                event->status = closing;

/*
 ** Processing Section
 */
               
            call_message.release = EVENT_STRUCTURE_RELEASE;
            call_message.version = EVENT_STRUCTURE_VERSION;
            call_message.time_created = time(NULL);
               
            strncpy(call_message.name,event->name,sizeof(call_message.name));
            strncpy(call_message.process_type,eTypes[event->type],sizeof(call_message.process_type));
            call_message.filedes = event->fd;
            strncpy(call_message.path,event->path,sizeof(call_message.path));
            strncpy(call_message.ipAddr,event->ipAddress,sizeof(call_message.ipAddr));
            call_message.processingPtr = event->eventHandlerPtr;
               
            switch(event->status)
               {
                  case listening:
                       SCstart_event_process(event);
                       continue;
               
                  case closing:
                       call_length = event->bufferLth;
                       (event->cptr)(&call_message,
                                      event->buffer,
                                     &call_length);
                       SCdelete_event(kQueueID,
                                      event,
                                      (int) eventList[i].ident,
                                      &call_message);
                       continue;
                       
                  case error:
                       return_code = (event->fptr)(&call_message,
                                                   event->buffer,
                                                   &call_length);
                       continue;

                  default:
                        event->status = processing;
                        call_length = event->bufferLth;
                        filedes = event->fd;
                       
                        error_code = (event->fptr)(&call_message,
                                                    event->buffer,
                                                    &call_length);
                       
                        event->eventHandlerPtr = call_message.processingPtr;
                       
                       /*** DEBUG CODE **/
                       fprintf(stdout,"%s: Returned from processing function.[%d]",
                               __FUNCTION__,
                               error_code);
                       /*** DEBUG CODE **/
                       
                        if (error_code == EVENT_CONNECTION_CLOSED)
                           {
                                event->status = closing;
                                call_length = event->bufferLth;
                                
                                (event->cptr)(&call_message,
                                              event->buffer,
                                              &call_length);
                                
                                SCdelete_event(kQueueID,
                                               event,
                                               (int) eventList[i].ident,
                                               &call_message);
                                continue;
                           
                            }
                       
                        if (error_code != 0)
                            return_code = (event->eptr)(&call_message,
                                                        event->buffer,
                                                        &call_length);
                       

              }

               
/*
 **** End of Processing Section
 */

          }
     
    bzero(eventList,sizeof(eventList));
    return_code = 0;
    
    }
    
return(NULL);

}