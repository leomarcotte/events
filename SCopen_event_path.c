//
//  open_event_path.c
//  events
//
//  Created by Leo Marcotte on 6/16/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>


void     SCtrace();

char * SCget_config(char * name);

int SCopen_event_path(const char *name, char * inPath, char *ioControls)
{
    
    static FILE *inFile;
    time_t  rawtime;
    struct  tm *info;
    
    assert(inPath != NULL);
    assert(ioControls != NULL);
    
    time( &rawtime );                                           // localhost.queue
    info = localtime( &rawtime );
    
    
    if (strlen(inPath) == 0)
    {
        fprintf(stdout,"%s %s: Input Queue path not found.\nAdd 'inpath' to config file.",
                __TIME__, __FUNCTION__);
        return(-2);
    }
    
    inFile = fopen(inPath, ioControls);
    
    if (inFile == NULL)
    {
        fprintf(stdout,
                "%s %s: (fopen): Error occurred opening %s\n%s\n",
                __TIME__, __FUNCTION__,
                inPath,
                strerror(errno));
        
        return(-3);
    }

    
    fprintf(stdout,
            "%s QUEUE %s [%d] path:%s opened\n",
            __TIME__,
            name,
            fileno((inFile)),
            inPath);
    
    return(fileno((inFile)));
    
}

