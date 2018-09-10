/*
** set and gets some static values
*/

#include "combine.h"
#include "count.h"

static int  s_do_cache= 1;
static int  s_user_reload=0;
static int  s_expires=  300;
static char *s_section= (char *) NULL;

/* set the value of s_do_cache to the passed value */
/* no checking is done */
void setCache(cache)
int
    cache;
{
    s_do_cache=cache;
}

/* return the last set value of s_do_cache */
int doHTTPcache()
{
    return(s_do_cache);
}


void setUserReload(i)
int
    i;
{
    s_user_reload=i;
}

int getUserReload()
{
    return (s_user_reload);
}


/* set the value of s_expires to the passed value */
/* no checking is done */
void setExpire(exp)
int
    exp;
{
    s_expires=exp;
}

/* return the last set value of s_expires */
int getExpireOffset()
{
    return(s_expires);
}

/*  
** dup passed section to s_section
*/
void setcfgSection(section)
char
    **section;
{
    if (s_section != (char *) NULL)
    {
        (void) free(s_section);
        s_section=(char *) NULL;
    }
    if (*section)
        s_section=mystrdup(*section);  /* we'll detect later if it fails here */
}

/*
** return pointer to s_section
*/
char *getcfgSection()
{
    return (s_section);
}

