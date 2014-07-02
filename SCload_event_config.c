//
//  SCload_event_config.c
//  events
//
//  Created by Leo Marcotte on 6/19/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
#include "config.h"

static CONFIG_DATA *eventConfig;

int SCload_event_config(char reload)
{
    CONFIG_DATA *config;
    FILE *configFile;
    int cfgBytes;
    char path[256];
    char work[256];
    char line[512];
    int  rcdCount = 0;
    char *lwork, *vwork;
    struct stat cfgInfo;
    
    getcwd(work,sizeof(work));
    
    if (strncmp(work,"/usr",4) == 0)
        strcpy(work,"/ServiceChi/websites/mcWeb");
    
    strcpy(path,work);
    
    // Build path to config file
    
    strcat(path,"/config");						// Check file to see if modified, if
    // not, return address
    
    stat(path, &cfgInfo);
    
    fprintf(stdout,"CONFIG path: %s\n",path);
    
    configFile = fopen(path,"r");
    
    if (configFile == NULL)						// if not config file, return default
    {
        fprintf(stdout,
                "%s: No configuration set located at \n%s - %s\n",
                __FUNCTION__,
                strerror(errno),
                path);
         exit (EXIT_FAILURE);
    }
    
    rcdCount = 2;
     while(fgets(line, 511, configFile) != NULL)
         rcdCount++;
    
    cfgBytes = rcdCount * sizeof(CONFIG_DATA);
                                                        // if reload flag is set, clear previous get
    if (reload == 'R' &&
        config != NULL)
        free(config);
    
    eventConfig = malloc(cfgBytes);
    
    if (eventConfig == NULL)
       {
        fprintf(stdout,"%s %s: Unable to allocate space for configuration data. %s. Aborting\n",
                __TIME__,__FUNCTION__,
                strerror(errno));
        exit(errno);
       }
    
    config = eventConfig;
    
    memset((char*) config,'\0',sizeof(CONFIG_DATA));
    config->num = 0;
    time(&config->fileTime);
    
    rewind(configFile);
    
    strncpy(config->entry.label,
            "config_path",
            sizeof(config->entry.label));
    
    strncpy(config->entry.value,
            path,
            sizeof(config->entry.value));
    
                                                    // Note, first entry is path name of the config file.
    config->num = rcdCount;
    config++;
                                                    // Read config data from config file
    while(fgets(line, 511, configFile) != NULL)     // into the configuration memory of process.
    {
        if (line[0] == '\n')
            continue;
       
        lwork = strtok(line,"=");
        vwork = strtok(NULL,"\n\r");
        
        strncpy(config->entry.label,
                lwork,
                sizeof(config->entry.label));
        
        strncpy(config->entry.value,
                vwork,
                sizeof(config->entry.value));
        
        if (setenv(config->entry.label,config->entry.value,1) != 0)
            fprintf(stdout,"%s %s: Error setting %s -> %s into environment\n%s\n",
                    __TIME__, __FUNCTION__,
                    config->entry.label,
                    config->entry.value,
                    strerror(errno));
        
        if (rcdCount < 1)
            break;
        
        rcdCount--;
        config++;
    }
    
    
    fclose(configFile);
    
    return(EXIT_SUCCESS);
}