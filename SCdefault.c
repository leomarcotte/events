//
//  SCdefault.c
//  events
//
//  Created by Leo Marcotte on 6/24/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include "libevent.h"

int SCerror_function(EVENT_STRUCTURE * message, int fd, char * buffer, int *bufferLth)
{
    fprintf(stdout,"%s %s: ERROR [%s-%d] %s\n%s[%d]\n"
            ,__TIME__,
            message->name,
            message->process_type,
            message->return_code,
            message->path,
            message->message,
            message->message_length);
    
    return(0);
}

int SCclose_function(EVENT_STRUCTURE * message, int fd, char * buffer, int *bufferLth)
{
    fprintf(stdout,"%s %s: has been enacted\n",__TIME__, __FUNCTION__);
    return(0);
}

int SCdefault_process(EVENT_STRUCTURE* message, int fd, char * buffer, int *bufferLth)
{
    fprintf(stdout,"%s %s: has been enacted\n",__TIME__,__FUNCTION__);
    return(0);
}

int SCdefault(EVENT_STRUCTURE message[], int *fd,  char buffer[], int *length)
{
    
    fprintf(stdout,"%s %s: Default processing enabled\n",__TIME__,__FUNCTION__);
    fprintf(stdout,"%s %s: FD:%d\n MESSAGE: %s[%d]\n",
            __TIME__,
            message->name,
            *fd,
            message->message,
            message->message_length);
    
    return(0);;
}

