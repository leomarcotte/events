//
//  SCreload_config.c
//  events
//
//  Created by Leo Marcotte on 6/26/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "libevent.h"

int SCload_event_config(char reload);


int SCreload_config(EVENT_STRUCTURE *message,
                    char * buffer,
                    int  *length)
{
    int error_code;
    fprintf(stdout,"%s %s:Configuration change noted: %s.\n",
            __TIME__,__FUNCTION__, message->path);
    
    error_code = SCload_event_config('R');
    
    if (error_code == 0)
        fprintf(stdout,"%s: Configuration has been updated.\n",__FUNCTION__);
    
    return(0);
    
}
