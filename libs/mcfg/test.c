#include <stdarg.h>
#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>
#include <signal.h>
#include <termio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>

#include "mcfg.h"

static int doSection(char *sec_name);
static int doParameter(char *param_name,char *param_value);

static char
    *cursec=(char *) NULL;

static void RegSection(s)
char
    **s;
{
    if (cursec != (char *) NULL)
    {
        (void) free(cursec);
        cursec=(char *) NULL;
    }
    if (*s)
        cursec=strdup(*s);
}

/*
**  
**
**  Parameters:
**
**
**  Return Values:
**
**
**  Limitations and Comments:
**
**
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Apr-10-1998    
*/


char *GetCurrentSection()
{
    return(cursec);
}


static int doSection(char *sec_name)
{
    int
        rc=0;
    RegSection(&sec_name);

    /*
    (void) fprintf(stderr,"[%s]\n",sec_name);
    */
    return(rc);
}

static int doParameter(char *param_name,char *param_value)
{
    if (strcmp(param_name,param_value) == 0)
        (void) fprintf(stderr," (%s)\"%s\"\n",
                       GetCurrentSection(),param_name,param_value);
    else
        (void) fprintf(stderr," (%s) \"%s\" \"%s\"\n",
                       GetCurrentSection(),param_name,param_value);
    return(0);
}
int main (int argc,char **argv) 
{
    int
        rc;

    if (argc != 2)
    {
        (void) fprintf(stderr,"usage: %s <conf file>\n",argv[0]);
        exit(0);
    }
    rc=paramProcess(argv[1],NOT_MS_STYLE,doSection,doParameter);
    if (rc < 0)
    (void) fprintf(stderr," rc=%d\n",rc);
    exit(0);
}
