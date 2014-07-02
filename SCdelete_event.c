//
//  SCdelete_event.c
//  events
//
//  Created by Leo Marcotte on 6/20/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

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

int SCdel_eventPtr(EVENT_LIST **event);

int SCgetevent_kqueueID();

int SCdelete_event(int kQueueID, EVENT_LIST *event, int fd, void *call_message)
{
    struct kevent evSet;
    
                                                                        // Remove FD from kqueue
    EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);

    if (kevent(kQueueID,
               &evSet,
               1,
               NULL,
               0,
               NULL) == -1)
        fprintf(stdout,
                "%s %s: Error removing %s[%d] from event queue\n%s\n",
                __TIME__, __FUNCTION__,
                event->name,
                fd,
                strerror(errno));
    
     close(fd);
                                                                        // Get event table address
                                                                        // Tell log we are disconnecting
    fprintf(stdout,
              "%s %s: %s has been disconnected.\n",
              __TIME__,
              event->name,
              event->path);
    
     fflush(stdout);
                                                          // close the file descriptor
    
     SCdel_eventPtr(&event);
    
     return(0);
}

