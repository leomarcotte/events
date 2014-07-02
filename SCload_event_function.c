//
//  SCload_event_function.c
//  events
//
//  Created by Leo Marcotte on 6/19/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
char *SCenviron(char *name);

void *SCload_event_function(char * indexName)
{
    void    *handle;
    int (*fptr)(char *message,                  // Function to load from dynamic library speced in
                int  *fd,                       // config ie: name_procedure
                char * buffer,                  // and used to transfer control to once event is
                int  *length);                  // notified.

    char        path[256];
    char        procedure[64];
    char        envVar[32];
    
    bzero(path,sizeof(path));
    bzero(procedure,sizeof(procedure));
    bzero(envVar,sizeof(envVar));
    fptr = NULL;
    handle = NULL;
    
                                                /*** procedure name    ***/
    strncpy(envVar,indexName,sizeof(envVar));
    strncat(envVar,"_procedure",sizeof(envVar) - strlen(envVar) - 1);

    strncpy(procedure,SCenviron(envVar),sizeof(procedure));
    
                                                /*** library name    ***/
    strncpy(envVar,indexName,sizeof(envVar));
    strncat(envVar,"_library",sizeof(envVar) - strlen(envVar) - 1);
    strncpy(path,SCenviron(envVar),sizeof(path));
    
    if (strlen(path) == 0)                                           /* Not specidied, use self  */
        handle = RTLD_SELF;
    else
                                                                    /* open the defined library  */
        handle = dlopen(path, RTLD_LOCAL | RTLD_LAZY);
        
    if (handle == NULL)
        {
         fprintf(stdout,"%s %s\n loading %s from %s\n",__TIME__,dlerror(),procedure,path);
         handle = RTLD_SELF;
        }
    
   fptr = dlsym(handle, procedure);
    
    if (fptr == NULL)
    {
         fptr = dlsym(RTLD_SELF, "SCdefault");
        if (fptr == NULL)
            return(NULL);
        
    }

    
    return((int*) fptr);
}