//
//  SCvalidate_event.c
//  events
//
//  Created by Leo Marcotte on 6/20/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SCevents.h>


int SCvalidate_event(EVENT_LIST *event)
{
    
#include "SCevent_keywords.h"
   
    int i;
    int typecheck;
    char work[2048];
    bzero(work,sizeof(work));
    
 
if (strlen(event->name) == 0)
    strcpy(work,"ERROR: event NAME not provided.\n");
    
if (event->fd == 0)
    strcat(work,"ERROR: opening event path. FD not provided\n");
    
if (strlen(event->path) == 0)
    strcat(work,"ERROR: event PATH not provided.\n");
    
if (event->type == file_event      ||
    event->type == queue_event     ||
    event->type == config_event    ||
    event->type == websocket_event ||
    event->type == tcp_event       ||
    event->type == automation_event)
     typecheck = (int) event->type;
else
    strcat(work,"ERROR: event type is outside of range (0-6)\n");
    
if (event->type == file_event  ||
    event->type == queue_event ||
    event->type == config_event)
    {
     for(i=0;i < 5;i++)
        if (strncmp(event->ioCtl,ioCtlDesignations[i],strlen(ioCtlDesignations[i])) == 0)
             break;
    
     if (i == 5)
        strcat(work,"ERROR: event ioctl is not a correct value\n");
    }
    
if (event->type == websocket_event ||
    event->type == tcp_event)
    if (event->port == 0)
        strcat(work,"ERROR: No port is defined for this communications event\n");
    
if (event->bufferLth > 0)
    if (event->buffer == NULL)
        strcat(work,"ERROR: No buffer is defined for this event\n");
    
if (strlen(work) > 0)
   {
    fprintf(stdout,
            "%s %s: Configuration errors for entry %s detected.\n%s",
            __TIME__, __FUNCTION__,
            event->name,
            work);
    return(EXIT_FAILURE);
   }
    
return(EXIT_SUCCESS);
    
}