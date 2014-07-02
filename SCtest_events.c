//
//  SCtest_events.c
//  events
//
//  Created by Leo Marcotte on 6/19/14.
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

int test_config(EVENT_STRUCTURE *message,
               char * buffer,
               int  *length)
{
    fprintf(stdout,"Config has been notified of change to configuration. %s\nFD=%d",
            message->name,
            message->filedes);
    sprintf(buffer,"test_config1 has been called! Returning notifiction\nFD: %d - %s",
            message->filedes,
            message->name);
    *length = (int) strlen(buffer);
    return(0);
    
}


int test_file1(EVENT_STRUCTURE *message,
               char * buffer,
               int  *length)
{
    fprintf(stdout,"File 1 has been notified of event %s\nFD=%d",
            message->name,
            message->filedes);
    sprintf(buffer,"test_file1 has been called! Returning notifiction\nFD: %d - %s",
            message->filedes,
            message->name);
    *length = (int) strlen(buffer);
    return(0);
    
}

int test_file2(EVENT_STRUCTURE *message,
               char * buffer,
               int  *length)
{
    fprintf(stdout,"File 2 has been notified of event %s\nFD=%d",
            message->name,
            message->filedes);
    sprintf(buffer,"test_file2 has been called! Returning notifiction\nFD: %d - %s",
            message->filedes,
            message->name);
    *length = (int) strlen(buffer);
    return(0);
    
}

int test_readsocket1(EVENT_STRUCTURE *message,
                     char * buffer,
                     int  *length)
{


    fprintf(stdout,"Socket 1 has been notified of event FD=%d",
        message->filedes);
    

    return(0);
    
}

int test_echo(EVENT_STRUCTURE *message,
             char * buffer,
              int  *length)
{
    
    int lth;
    
    fprintf(stdout,"Echo Testing entered on event FD=%d\n",message->filedes);
    fflush(stdout);
    
    lth = *length;
    
    lth = (int) read (message->filedes, buffer, lth);
    
    switch(lth < 0 ? EXIT_FAILURE : EXIT_SUCCESS)           // or 1 : 0
    {
        case EXIT_FAILURE:                                  /* Read error. */
            perror("tcp_read_from_client");
            
            if (errno == EINTR)
            {
                fprintf(stdout,"Interrupt occurred during read.  continuing execution\n");
                return(EXIT_SUCCESS);
            }
            return(EXIT_FAILURE);
            
        case EXIT_SUCCESS:
            
            if (strncmp(buffer,"quit",4) == 0)
               {
                send(message->filedes, "Good Bye!\n", 10, 0);
                fprintf(stdout,"%s: logout requested .\n",__FUNCTION__);
                fflush(stdout);
                return(EVENT_CONNECTION_CLOSED);
               }
            
            if (lth == 0)
            {
                fprintf(stdout,"%s: Connection closed by client.\n",__FUNCTION__);
                fflush(stdout);
                return(EVENT_CONNECTION_CLOSED);
            }
            
            fprintf(stdout,"RECEIVED: %s",buffer);
            
            send(message->filedes, buffer, lth, 0);
            
            fprintf(stdout,"SENT    : %s",buffer);
            
            return(EXIT_SUCCESS);

    }
    
return(EXIT_FAILURE);
    
}


int test_timer(EVENT_STRUCTURE *message,
               char * buffer,
               int  *length)
{
    fprintf(stdout,"Timer has been notified of event %s\nFD=%d",
            message->name,
            message->filedes);
            
    return(0);
}
