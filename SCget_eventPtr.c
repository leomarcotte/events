//
//  SCget_eventPtr.c
//  events
//
//  Created by Leo Marcotte on 6/20/14.
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

char * SCenviron(char * name);

char **SCstoredEventAddresses;
int  SCnumberEventAddresses;


char **SCgeteventAddressList()
{
    return (SCstoredEventAddresses);
}

int SCget_eventPtr(EVENT_LIST **event)
{
                                                        // Allocate Event Table address
    
    *event = malloc(sizeof(EVENT_LIST));
    
    if (*event == NULL)
    {
        fprintf(stdout,"%s %s: Unable to allocate memory for buffer. Length=%lu\n%s\n",
                __TIME__, __FUNCTION__,
                sizeof(EVENT_LIST),
                strerror(errno));
        
        return(EXIT_FAILURE);
    }
    
    bzero(*event,sizeof(EVENT_LIST));
    
    memcpy(*event,"Leo&Amy",7);
    
    return(EXIT_SUCCESS);
}


void SCinitEventAddressList()
{
    int i;
    int maxEvents;
                                                                        // Read event count from table
    SCnumberEventAddresses = 0;
    
    maxEvents = atoi(SCenviron("MAX_EVENTS"));
    
    if (maxEvents == 0)
        maxEvents = 100;
    
    
    SCstoredEventAddresses = malloc(maxEvents * sizeof(SCstoredEventAddresses));
 
    if (SCstoredEventAddresses == NULL)
    {
        fprintf(stdout,"%s %s: Unable to allocate memory for SCnumberAddresses.\n%s\n",
                __TIME__, __FUNCTION__,
                strerror(errno));
        
        exit(errno);
    }

    SCnumberEventAddresses = maxEvents;
    
    for(i=0;i< maxEvents;i++)
        SCstoredEventAddresses[i] = NULL;
    
    return;
}


