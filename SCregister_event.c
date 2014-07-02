//
//  SCregister_event.c
//  events
//
//  Created by Leo Marcotte on 6/17/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/event.h>
#include <unistd.h>
#include "SCevents.h"


int SCregister_event(int kqfd, struct kevent *changelist, int *changeCount)
{
    // the following kevent() call is for registering events
    int ret = 0;
    int i;
    
    for(i=0;i < MAX_EVENTS;i++)
        if (changelist[i].ident <= 0)
            break;
    
    ret = kevent(kqfd,        // kqueue file descriptor
                 changelist,  // array of kevent structures
                 i,           // number of entries in the changelist array
                 NULL,        // array of kevent structures (for receiving)
                 0,           // number of entries in the above array
                 NULL);       // timeout
    
    if (ret < 0)
    {
        fprintf(stdout,"%s: Unable to enable event list",__FUNCTION__);
        perror("kqueue");
        return(-1);
    }
    
    return(i);
}