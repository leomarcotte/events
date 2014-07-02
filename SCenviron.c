//
//  SCenviron.c
//  events
//
//  Created by Leo Marcotte on 6/20/14.
//  Copyright (c) 2014 Leo Marcotte. All rights reserved.
//

#include <stdio.h>
#include <string.h>
char *SCenviron(char *name)
{
extern char **environ;
char **env = NULL;
char *label = NULL;
char *value = NULL;
int   lth;
    
for (env = environ; *env; ++env)
    {
     label = *env;
     value = strstr(label,"=") + 1;
     lth   = (int) strlen(name);
        
     if ( strncasecmp(label,name,lth) == 0)
          return(value);
    }

return("");
}