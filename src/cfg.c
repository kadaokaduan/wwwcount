/*
**  routines to handle Count 2.5 configuration file
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Dec-27-1998    first cut
*/

#include <combine.h>
#include "count.h"
#include "cdebug.h"

#ifdef SYS_WIN32
#include "configNT.h"
#elif defined( __VMS )
#include "configVMS.h"
#else
#include "config.h"
#endif


#include <mcfg.h>

#ifdef __VMS
#define DIR_SEPARATOR "" /* Already have ] at end of _DIR macros */
#else
#define DIR_SEPARATOR "/"
#endif


/*
** global in this file. member of this struct is accessible via some function
** calls. a way to avoid globals.
*/
static Cfg *s_cfg=(Cfg *) NULL;

/* private protos */
/*
static void printConfigError _Declare ((int err_type));
*/

static int  handlecfgSection _Declare ((char *section));
static int  handlecfgParam   _Declare ((char *param,char *value));
static Cfg  *initCfg         _Declare ((void));
static void setCfgBoolean    _Declare ((char *value,int *set));

/*
static void printCfg         _Declare ((void));
*/


void ParseConfig ()
{
    int
        rc=0;

    char
        buf[MaxLineLength+1];

    *buf='\0';

#ifdef SYS_WIN32

    /*
    ** g_sZ_vbase has a trailing \ at the end
    */
    safeStrcpy(buf,g_sZ_vbase,sizeof(buf)-1);
    safeStrcat(buf,CONFIG_DIR,sizeof(buf)-1,sizeof(buf),strlen(buf));
    safeStrcat(buf,"/",sizeof(buf)-1,sizeof(buf),strlen(buf));
    safeStrcat(buf,CONFIG_FILE,sizeof(buf)-1,sizeof(buf),strlen(buf));


#else

    safeStrcpy(buf,CONFIG_DIR,sizeof(buf)-1);
    safeStrcat(buf,DIR_SEPARATOR,sizeof(buf)-1,sizeof(buf),strlen(buf));
    safeStrcat(buf,CONFIG_FILE,sizeof(buf)-1,sizeof(buf),strlen(buf));
#endif

    /*
    ** initialize Cfg struct
    */
    s_cfg=initCfg();
    if (s_cfg == (Cfg *) NULL)
    {
        StringImage("malloc failed at cfg.c:ParseConfig()");
        exit(0);
    }

   Debug2("cfg file=%s",buf,0);
   rc=paramProcess(buf,NOT_MS_STYLE,handlecfgSection,handlecfgParam);
   switch (rc)
   {
       case -1:
       {
           StringImage("Unable to open config file for reading");
           exit(0);
           break;   /* won't be here */
       }

       case -2:
       {
           StringImage("malloc failed while parsing config file");
           exit(0);
           break; /* won't be here */
       }

       case -3:
       {
           StringImage("parse error in config file");
           exit(0);
           break;   /* won't be here */
       }
   }


   /*
   printCfg();
   */

}


/*
** initialize the Cfg structure
*/
static Cfg *initCfg()
{
    Cfg
        *cfg;

    char
        *func="cfg.c:initCfg() -";

    cfg=(Cfg *) malloc(sizeof(Cfg));
    if (cfg == (Cfg *) NULL)
    {
        (void) fprintf(stderr,"malloc failed at: %s\n",func);
        return ((Cfg *) NULL);
    }

    cfg->version=(char *) NULL;
    cfg->auto_file_creation=False;
    cfg->strict_mode=True;
    cfg->allow_rgb_database=False;
    cfg->count_reload=False;
    cfg->log_error_messages=True;
    cfg->show_error_messages_to_browsers=True;
    cfg->ignore_ips=NULL;
    cfg->auth_hosts=NULL;

    return (cfg);
}

/*
** handle section of the cfg file. just store it
*/
static int handlecfgSection(section)
char
    *section;
{
    setcfgSection(&section);
    return(0);
}

/*
** main routine to handle the prameters in the cfg file
*/
static int handlecfgParam(param,value)
char
    *param;
char
    *value;
{
    char
        *p,
        *v,
        *cursec;

    cursec=getcfgSection();
    if (cursec == NULL || s_cfg == NULL || param == NULL)
        return (-1);

    if (mystrcasecmp("version",cursec) == 0)
    {
        s_cfg->version=mystrdup(param);
        if (s_cfg->version == (char *) NULL)
            return (-1);
    }
    else if (mystrcasecmp("options",cursec) == 0)
    {
        p=param;
        v=mystrtok(p,"=");
        if (v != NULL)
        {
            if (mystrcasecmp(v,"auto_file_creation") == 0)
            {
                p=(char *) NULL;
                v=mystrtok(p,"=");
                if (v != (char *) NULL)
                    setCfgBoolean(v,&s_cfg->auto_file_creation);
            }
            else if (mystrcasecmp(v,"strict_mode") == 0)
            {
                /* left out, added Feb-01-1999 */
                p=(char *) NULL;
                v=mystrtok(p,"=");
                if (v != (char *) NULL)
                    setCfgBoolean(v,&s_cfg->strict_mode);
            }
            else if (mystrcasecmp(v,"allow_rgb_database") == 0)
            {
                p=(char *) NULL;
                v=mystrtok(p,"=");
                if (v != (char *) NULL)
                    setCfgBoolean(v,&s_cfg->allow_rgb_database);
            }
            else if (mystrcasecmp(v,"count_reload") == 0)
            {
                p=(char *) NULL;
                v=mystrtok(p,"=");
                if (v != (char *) NULL)
                    setCfgBoolean(v,&s_cfg->count_reload);
            }
            else if (mystrcasecmp(v,"log_error_messages") == 0)
            {
                p=(char *) NULL;
                v=mystrtok(p,"=");
                if (v != (char *) NULL)
                    setCfgBoolean(v,&s_cfg->log_error_messages);
            }
            else if (mystrcasecmp(v,"show_error_messages_to_browsers") == 0)
            {
                p=(char *) NULL;
                v=mystrtok(p,"=");
                if (v != (char *) NULL)
                    setCfgBoolean(v,&s_cfg->show_error_messages_to_browsers);
            }


        }
    }
    else if (mystrcasecmp("ignore ips",cursec) == 0)
        additemToList(&s_cfg->ignore_ips,param);
    else if (mystrcasecmp("authorized",cursec) == 0)
        additemToList(&s_cfg->auth_hosts,param);

    return(0);
}

/*
** ignoreIP()
**    returns True if ignore, False otherwise
*/
int ignoreIP(remote_ip)
char
    *remote_ip;
{
    Sllist
        *ig_ip,
        *llp;

    int
        rc=False;

    if (s_cfg == (Cfg *) NULL)
    {
        /*
        ** should write some message
        */
        return (rc);
    }
    ig_ip=getcfgIgips();
    for (llp=ig_ip;llp;llp=llp->next)
    {
        rc=CheckRemoteIP(remote_ip,llp->item);
        if (rc == True)
            break;
    }

    return (rc);
}

/*
** hostAuthorized()
**    returns True if Authorized, False otherwise
*/
int hostAuthorized(host)
char
    *host;
{
    Sllist
        *auth_hosts,
        *llp;

    int
        rc=False;

    if (s_cfg == (Cfg *) NULL)
    {
        /*
        ** should write some message
        */
        return (rc);
    }
    auth_hosts=getcfgAhs();
    for (llp=auth_hosts;llp;llp=llp->next)
    {
        rc=isinname(host,llp->item);
        if (rc == True)
            break;
    }

    return (rc);
}

/*
** print the Cfg struct
*/

#if 0
static void printCfg()
{
    Sllist
        *ll,
        *ip;

    (void) fprintf(stderr,"[version]\n");
    (void) fprintf(stderr," %s\n",getcfgVersion());
    (void) fprintf(stderr,"[options]\n");
    (void) fprintf(stderr," auto_file_creation=%d\n",getcfgAutoFileCreation());
    (void) fprintf(stderr," strict_mode=%d\n",getcfgStrictMode());
    (void) fprintf(stderr," allow_rgb_database=%d\n",getcfgArgbDatabase());
    (void) fprintf(stderr," count_reload=%d\n",getcfgCountReload());
    (void) fprintf(stderr," log_error_messages=%d\n",getcfgLogErrmsg());
    (void) fprintf(stderr," show_error_messages_to_browsers=%d\n",
                                getcfgShowErrmsg());                            
    (void) fprintf(stderr,"[ignore ips]\n");
    ip=getcfgIgips();
    for (ll=ip;ll;ll=ll->next)
    {
        (void) fprintf(stderr," %s\n",ll->item);
    }
    (void) fprintf(stderr,"[authorized]\n");
    ip=getcfgAhs();
    for (ll=ip;ll;ll=ll->next)
    {
        (void) fprintf(stderr," %s\n",ll->item);
    }

}

#endif /* if 0 */


/*
** setCfgBoolen()
*/
static void setCfgBoolean(value,set)
char
    *value;
int
    *set;
{
    if (value != (char *) NULL)
    {
        if ((mystrcasecmp(value,"y") == 0)      ||
            (mystrcasecmp(value,"yes") == 0)    ||
            (mystrcasecmp(value,"1") == 0)      ||
            (mystrcasecmp(value,"true") == 0))
        {
            *set=True;
        }
        else
        {
            if ((mystrcasecmp(value,"n") == 0)      ||
                (mystrcasecmp(value,"no") == 0)    ||
                (mystrcasecmp(value,"0") == 0)      ||
                (mystrcasecmp(value,"false") == 0))
            {
                *set=False;
            }
        }
    }
}


/*
** functions to access static cfg struct from outside, better than globals
** anyway.
*/

char *getcfgVersion()
{
    if (s_cfg)
        return (s_cfg->version);
    else
        return (NULL);
}

int getcfgAutoFileCreation()
{
    if (s_cfg)
        return (s_cfg->auto_file_creation);
    else
        return (False);
}

int getcfgStrictMode()
{
    if (s_cfg)
        return (s_cfg->strict_mode);
    else
        return (True);
}

int getcfgArgbDatabase()
{
    if (s_cfg)
        return (s_cfg->allow_rgb_database);
    else
        return (False);
}

int getcfgCountReload()
{
    if (s_cfg)
        return (s_cfg->count_reload);
    else
        return (True);
}

int getcfgLogErrmsg()
{
    if (s_cfg)
        return (s_cfg->log_error_messages);
    else
        return(True);

}

int getcfgShowErrmsg()
{
    if (s_cfg)
        return (s_cfg->show_error_messages_to_browsers);
    else
        return(True);
}

Sllist *getcfgIgips()
{
    if (s_cfg)
        return (s_cfg->ignore_ips);
    else
        return(NULL);
}

Sllist *getcfgAhs()
{
    if (s_cfg)
        return (s_cfg->auth_hosts);
    else
        return(NULL);
}


