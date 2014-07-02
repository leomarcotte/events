//
//  SCdisplayEvent.c
//  events
//
//  Created by Leo Marcotte on 6/19/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include <sys/event.h>
#include <unistd.h>
#include <SCevents.h>

typedef struct {
    u_int       event;
    const char *description;
} SCEventDescriptions_t;

void SCdisplayEvent(struct kevent *event,
                    int nevents)
{
    int i;
    
#define NEVENTS 20
SCEventDescriptions_t SCEventDescriptions[] =
{
    { NOTE_ATTRIB, "attributes changed"                      },
    { NOTE_DELETE, "deleted"                                 },
    { NOTE_EXTEND, "extended"                                },
    { NOTE_LINK,   "link count changed"                      },
    { NOTE_RENAME, "renamed"                                 },
    { NOTE_REVOKE, "access revoked or file system unmounted" },
    { NOTE_WRITE,  "written"                                 },
    { NOTE_TRIGGER,"event to be triggered for output"        },
    { NOTE_FFNOP,  "ignore fflags"                           },
    { NOTE_FFAND,  "and fflags"                              },
    { NOTE_FFOR,   "or fflags"                               },
    { NOTE_FFCOPY, "mask for operations"                     },
    { NOTE_FFLAGSMASK,"mask for operations"                  },
    { NOTE_LOWAT,  "low water mark"                          },
    { NOTE_EXIT,   "process exited"                          },
    { NOTE_FORK,   "process forked"                          },
    { NOTE_EXEC,   "process exec'd"                          },
    { NOTE_SIGNAL, "process signal (ECFILT_SIGNAL)"          },
    { NOTE_EXITSTATUS,"exit status to be returned (child)"   },
    { NOTE_EXIT_DETAIL,"process details on exit"             }
};

    for (i = 0; i < NEVENTS; i++)
        if (SCEventDescriptions[i].event & event->fflags)
            fprintf(stdout,
                    "%lu [%s] \t%ld\n",
                    event->ident,
                    SCEventDescriptions[i].description,
                    event->data);

}