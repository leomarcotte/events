//
//  SCevent_keywords.h
//  events
//
//  Created by Leo Marcotte on 6/21/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#ifndef events_SCevent_keywords_h
#define events_SCevent_keywords_h

#include <sys/event.h>
char scEventKeywords[32];
// Defines the keywords used in config and display
static char *eTypes[] =
{
    "not defined",
    "websocket",
    "tcp",
    "file",
    "queue",
    "config",
    "automation",
    ""
};
strncpy(scEventKeywords,eTypes[0],sizeof(scEventKeywords));

// Defines keywords used in config file and display
static char *ioTypes[] =
{
    "not defined",
    "read",
    "readwrite",
    "write",
    "append",
    "notify",
    ""
};

strncpy(scEventKeywords,ioTypes[0],sizeof(scEventKeywords));

// Defines actual values for the keywords
char *ioCtlDesignations[] =
{   "not defined",
    "r",
    "w+",
    "w",
    "a",
    "w+",
    ""
};
strncpy(scEventKeywords,ioCtlDesignations[0],sizeof(scEventKeywords));



int16_t      keFilter       = EVFILT_READ;
int16_t      keFlag         = EV_ADD | EV_ENABLE | EV_CLEAR;
int32_t      keFFlags       = NOTE_WRITE;
int16_t      keFileFlag     = EV_ADD | EV_ENABLE | EV_CLEAR;
int16_t      kesocketFlag   = EV_ADD | EV_ENABLE | EV_CLEAR;
int16_t      kesocketFilter = EVFILT_READ;
int16_t      kefileFilter   = EVFILT_VNODE;
int32_t      kefileFFlags   = NOTE_WRITE;
int32_t      kesocketFFlags = NOTE_EXIT;

if (keFilter | keFlag | keFFlags || keFileFlag || kesocketFlag || kesocketFilter || kefileFilter || kefileFFlags | kesocketFFlags)
;
#endif
