//
//  SCdel_eventPtr.c
//  events
//
//  Created by Leo Marcotte on 6/23/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/event.h>
#include <unistd.h>
#include <dlfcn.h>
#include "SCevents.h"

char *SCenviron(char * name);

void *SCgeteventAddressList();

int SCdel_bufferPtr(char **buffer);

int SCdel_eventPtr(char **event)
{
    EVENT_LIST *ev;

    ev = (EVENT_LIST*) *event;
    
    if (ev->buffer != NULL)                                         // Free the buffer if one allocated
        SCdel_bufferPtr(&ev->buffer);
    
    if (*event != NULL)
        free(*event);
    
    *event = NULL;                                                 // Clear from event Address store
    
    return(EXIT_SUCCESS);
}
