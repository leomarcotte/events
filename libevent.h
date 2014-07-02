//
//  libevent.h
//  events
//
//  Created by Leo Marcotte on 6/24/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//
//  External Header file for using libevent routines.

#ifndef events_libevent_h
#define events_libevent_h

#ifndef tcp_tcp_h
extern void *TCP_transasction_storagePtr;
#endif

#ifdef tcp_tcp_h
void *TCP_transasction_storagePtr;
#endif

#ifndef websocket_websocket_h
extern void *WS_transasction_storagePtr;
#endif

#ifdef websocket_websocket_h
void *WS_transasction_storagePtr;
#endif

char *SCenviron(char * name);

#include <time.h>
#define EVENT_MSG_RETURN        -256
#define EVENT_CONNECTION_CLOSED -255

typedef enum
{
    initApp       = 0,                                      // initApp
    processApp    = 1,                                      // message area contains continuation pkg
    callFunction  = 2,                                      // message area contains the function name
    closeApp      = 3,                                      // message area contains FD
    openNewApp    = 4,                                      // message area contains path
    notifyApp     = 5,                                      // message area contains name
    notifyType    = 4                                       // message area contains type
}APPREQUESTS;

#define EVENT_STRUCTURE_RELEASE 1
#define EVENT_STRUCTURE_VERSION 1
typedef struct
{
    int             release;                                // Release level
    int             version;                                // Version level of structure
    time_t          time_created;                           // Time event created
    char            name[32];                               // Name of event
    char            path[256];                              // Path/Host name
    int             port;                                   // port numebr connected to
    char            ipAddr[64];                             // IP address of client
    char            process_type[32];                       // type of process
    int             filedes;                                // filedes of event
    APPREQUESTS     request;                                // requests to/from event procssing
    int             return_code;                            // response from application to event system
    void            *processingPtr;                         // Ptr provided by event handler and returned to it
    char            message[256];                           // free form message area
    int             message_length;                         // size of message area
} EVENT_STRUCTURE;

#endif
