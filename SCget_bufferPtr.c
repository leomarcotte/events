//
//  SCget_bufferPtr.c
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

char * SCenviron(char * name);

char **SCstoredBufferAddresses;
int  SCnumberBufferAddresses;

char **SCgetBufferAddressList()
{
    return (SCstoredBufferAddresses);
}

void SCinitBufferAddressList()
{
    int i;
    int maxBuffers;
    // Read event count from table
    SCnumberBufferAddresses = 0;
    
    maxBuffers = atoi(SCenviron("MAX_EVENTS")) * 2;
    
    if (maxBuffers == 0)
        maxBuffers = 200;
    
    
    SCstoredBufferAddresses = malloc(maxBuffers * sizeof(SCstoredBufferAddresses));
    
    if (SCstoredBufferAddresses == NULL)
    {
        fprintf(stdout,"%s %s: Unable to allocate memory for SCBufferAddresses.\n%s\n",
                __TIME__, __FUNCTION__,
                strerror(errno));
        
        exit(errno);
    }
    
    SCnumberBufferAddresses = maxBuffers;
    
    for(i=0;i< maxBuffers;i++)
        SCstoredBufferAddresses[i] = NULL;
    
    return;
}

int SCget_bufferPtr(char **buffer, int bufferLength)
{
    *buffer = malloc(bufferLength);
    
    if (*buffer == NULL)
    {
        fprintf(stdout,"%s %s: Unable to allocate memory for buffer. Length=%d\n%s\n",
                __TIME__, __FUNCTION__,
                bufferLength,
                strerror(errno));
        
        return(EXIT_FAILURE);
    }
    
    bzero(*buffer,bufferLength);
    memcpy(*buffer,"Leo&Amy",7);
    return(EXIT_SUCCESS);
    
}


