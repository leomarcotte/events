//
//  SCdel_bufferPtr.c
//  events
//
//  Created by Leo Marcotte on 6/23/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/event.h>
#include <unistd.h>
#include <dlfcn.h>
#include "SCevents.h"

char *SCenviron(char * name);

void **SCgetBufferAddressList();

int SCdel_bufferPtr(char **buffer)
{
    
    if (*buffer != NULL)
        free(*buffer);
    
    *buffer = NULL;
    
    return(EXIT_SUCCESS);
}
