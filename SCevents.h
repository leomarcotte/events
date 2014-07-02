//
//  SCevents.h
//  events
//
//  Created by Leo Marcotte on 6/15/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#ifndef events_SCevents_h
#define events_SCevents_h

#include <time.h>

#include "libevent.h"

#define DEFAULT_ERROR_FUNC "SCerror_function"
#define DEFAULT_CLOSE_FUNC "SCclose_function"
#define DEFAULT_PROC_FUNC  "SCdefault_process"

typedef enum {
    eventNotdefined   = 0,
    websocket_event   = 1,                      // defined as a websocket tcp process
    tcp_event         = 2,                      // defined as a straight tcp process
    file_event        = 3,                      // defined as a file
    queue_event       = 4,                      // defined asa queue mechanism
    config_event      = 5,                      // defined as a configuration file
    automation_event  = 6                       // a timed event
} EVENT_TYPE;

typedef enum {
    readFile         = 1,
    readWrite        = 2,
    writeFile        = 3,
    append           = 4,
    notification     = 5
} EVENT_IOCTL_TYPE;

typedef enum {
      notKnown       = 0,
      opened         = 1,
      listening      = 2,
      connected      = 3,
      processing     = 4,
      error          = 5,
      closing        = 6,
      locked         = 7
} EVENT_STATUS;

typedef struct {
    int           fd;                           // FD used in the kQueue. Needs to be unique
    char          name[32];                     // name to use to reference sybolically
    EVENT_TYPE    type;                         // Type of event (defines processing)
    EVENT_STATUS  status;                       // Status of event
    char          path[256];                    // Full path to the file or host
    char          ipAddress[64];                // IP address of connection (blank on files)
    char          current_dir[256];             // full pth to directory run in
    int           port;                         // If host, tcp port.. if auto.. waittime in seconds
    char          ioCtl[10];                    // Coupled with file (w+ = read/write, r = read)
    int (*fptr)(void *message,                  // Function to load from dynamic library speced in
                char * buffer,                  // config ie: name_procedure and used to
                int  *length);                  // transfer control to once event is notified.
                                                //
    int (*cptr)(void *message,                  // End Function to load from dynamic library and
                char * buffer,                  // called from the SCdelete_event routine to
                int  *length);                  // perform Application closeout
                
    int (*eptr)(void *message,                  // End Function to load from dynamic library and
                char * buffer,                  // called when a processing error is encoutered
                int  *length);                  // (i.e return a non-zero code to SCwait event
                

    time_t      last_use;                       // Time table last used
    void         *eventHandlerPtr;              // Ptr provided by event handler between sessions
    char         *buffer;                       // buffer associated with event
    int           bufferLth;                    // length of buffer associated with event
} EVENT_LIST;


#define NOTE_ALL NOTE_ATTRIB |\
NOTE_DELETE |\
NOTE_EXTEND |\
NOTE_LINK   |\
NOTE_RENAME |\
NOTE_REVOKE |\
NOTE_WRITE
#endif
