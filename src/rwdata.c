#ifdef __VMS
#include <fcntl.h>
#include <unixio.h>
#endif

#include "cdebug.h"
#include "combine.h"
#include "gdfonts.h"
#include "count.h"

#include <bigplus.h>

/* protos */
static void add1toData _Declare ((char *buf,int buflen));
static void getParts   _Declare ((char *allbuf,char *dbuf,char *ipbuf,
                                  int dbuf_size,int ipbuf_size));
/*
**  rwCounterDataFile()
**  function to read and write counter data from a flat file. the function
**  does the kernel locking on the file.
**
**  RCS
**      $Revision: 1.1.1.1 $
**      $Date: 2001/03/19 01:59:51 $
**  Return Values:
**      returns the buffer containing the counter digits in case of success.
**      in case of failure of some sort, the routine does not return, it
**      writes the error message image to the browser and exits with 0.
**
**  Parameters:
**
**
**  Side Effects:
**      the content of the counter datafile is modified.
**
**  Limitations and Comments:
**      The buffer returned points to a malloc'd space. the caller is
**      responsible to free this memory.
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-11-1997    first cut
**      ma_muquit@fccc.edu   Oct-30-1997    if content of datafile is 0,
**                                          data:IP was not written back
*/

char *rwCounterDataFile(filepath,
                        remote_ip,
                        ignore_site,
                        use_st,
                        start_value,
                        do_increment)
char
    *remote_ip;     /* remote IP, can be NULL, so check */
        
char
    *filepath;      /* full datafile path */

int
    ignore_site;    /* ignore this site or not */

int
    use_st;         /* use start value or not */

int
    start_value;    /* start value, used if use_st is true */

int
    do_increment;   /* should be counter be incremented */

{
#define IP_LENGTH   16
    int
        length,
        n,
        fd;

    int
        ipbuf_size,
        malloc_size;

    char
        tmpbuf[1024],
        *readbuf=(char *) NULL,
        *ipbuf=(char *) NULL,
        *dbuf=(char *) NULL;

    *tmpbuf='\0';

    /*
    ** open the file
    */
    Debug2("filepath=%s",filepath,0);
#ifdef SYS_WIN32
        fd=sopen(filepath,_O_RDWR|_O_CREAT,_SH_DENYWR,_S_IREAD|_S_IWRITE);
#else
        fd=open(filepath,O_RDWR|O_CREAT,0644);
#endif

    if (fd < 0) /* open failed */
    {
        if (getcfgAutoFileCreation() == True)
        {
            if (CheckFile(filepath) != 0)
            {
                /* tmpbuf is a static buff, we can use sizeof safely */
                safeStrcpy(tmpbuf,"Could not create data file: ",
                        sizeof(tmpbuf)-1);
                safeStrcat(tmpbuf,filepath,sizeof(tmpbuf)-1,
                        sizeof(tmpbuf),strlen(tmpbuf));
            }
            else
            {
                /* tmpbuf is a static buff, we can use sizeof safely */
                safeStrcpy(tmpbuf,"Could not write to counter file: ",
                        sizeof(tmpbuf)-1);
                safeStrcat(tmpbuf,filepath,sizeof(tmpbuf)-1,
                        sizeof(tmpbuf),strlen(tmpbuf));
            }
        }
        else
        {
            /* tmpbuf is a static buff, we can use sizeof safely */
            safeStrcpy(tmpbuf,"Could not write to counter file: ",
                    sizeof(tmpbuf)-1);
            safeStrcat(tmpbuf,filepath,sizeof(tmpbuf)-1,
                    sizeof(tmpbuf),strlen(tmpbuf));
        }
        Warning(tmpbuf);
        StringImage(tmpbuf);
        exit(0);
    }

    /*
    ** lock the file
    */
#ifdef SYS_UNIX
        SetLock(fd);
#endif
    
    /*
    ** try to read from file, rewind it first
    */
    lseek(fd,0L,0);

    /*
    ** malloc. If malloc fails, we'll be outta here
    */
    malloc_size=(MAX_DIGITS+IP_LENGTH)*sizeof(char)+1;

    /*
    ** holds digit:IP
    */
    readbuf=(char *) cMalloc(malloc_size);
    memset(readbuf,0,malloc_size);

    /*
    ** holds digits
    */
    dbuf=(char *) cMalloc(malloc_size);
    memset(dbuf,0,malloc_size);

    /*
    ** holds IP
    */
    ipbuf_size=IP_LENGTH*sizeof(char) + 1;
    ipbuf=(char *) cMalloc(ipbuf_size);
    memset(ipbuf,0,ipbuf_size);
    
    /*
    ** read from file
    */
    n=read(fd,readbuf,malloc_size);

    if (n > 0)  /* read something */
    {
        /*
        ** dbuf and ipbuf will be filled. note dbuf will be returned
        ** so it bust have only digits
        ** NOTE, it's possible ipbuf holds NULL, so be careful!!
        */
        getParts(readbuf,dbuf,ipbuf,malloc_size-1,ipbuf_size-1);

    if (getcfgCountReload() == False)   /* don't count reload */
    {
        /*
        ** if the IP is the ignore list already, why bother checking the
        ** ipbuf. Only check if the IP is in the count list.
        */
        if ((ignore_site == False) && (do_increment == True))
        {
            if (remote_ip != (char *) NULL)
            {
                /*
                ** compare the IP from the connection and IP in the file
                ** if they'r same, no need to increment or write back to the
                ** data file
                */

                /*
                ** it was wrong to check if ipbuf was NULL, it will not
                ** be NULL as it already points to a malloc'd space,
                ** we'r supposed to check if it the content is NULL
                ** Thanks to Wincom Administrator <admin@wcis.com>
                ** Nov-07-11997
                */
                if (*ipbuf != '\0')
                {
                    if (strncmp(ipbuf,remote_ip,IP_LENGTH) == 0)
                    {
                        /* 
                        ** same IP in connectin and in the file
                        */
                        do_increment=False;
                        ignore_site=True;
                    }
                    else
                    {
                        /* 
                        ** new IP, overwrite ipbuf with it, we'r gonna store it
                        ** in the data file
                        */
                        Debug2("-- ipbuf=%s, remote_ip=%s",ipbuf,remote_ip);
                        safeStrcpy(ipbuf,remote_ip,ipbuf_size-1);
                    }
                }
                else
                {
                    Debug2("ipbuf is NULL",0,0);
                    /*
                    ** remote IP is not NULL, and ipbuf is NULL, that means
                    ** the datafile has only digit part in it. overwrite
                    ** ipbuf with remote_ip
                    */
                    safeStrcpy(ipbuf,remote_ip,ipbuf_size-1);
                    
                }
            }
            else
            {
                Debug2("remote_ip is NULL, it should not be!",0,0);
            }
        }
    }
    else
    {
        /*
        ** site admin decided to count reload. now users have the option
        ** to increment or not
        */
        if (getUserReload() == False) /* user decided to ignore reload count*/
        {
                if ((ignore_site == False) && (do_increment == True))
                {
                    if (remote_ip != (char *) NULL)
                    {
                        Debug2(".... remote_ip=%s",remote_ip,0);
                        /*
                        ** compare the IP from the connection and IP 
                        ** in the file if they'r same, no need to increment or 
                        ** write back to the data file
                        */

                    /*
                    ** it was wrong to check if ipbuf was NULL, it will not
                    ** be NULL as it already points to a malloc'd space,
                    ** we'r supposed to check if it the content is NULL
                    ** Thanks to Wincom Administrator <admin@wcis.com>
                    ** Nov-07-11997
                    */
                    if (*ipbuf != '\0')
                    {
                        if (strncmp(ipbuf,remote_ip,IP_LENGTH) == 0)
                        {
                            /* 
                            ** same IP in connectin and in the file
                            */
                            Debug2("== ipbuf=%s, remote_ip=%s",ipbuf,remote_ip);
                            do_increment=False;
                            ignore_site=True;
                        }
                        else
                        {
                            /* 
                            ** new IP, overwrite ipbuf with it, we'r gonna 
                            ** store it in the data file
                            */
                            Debug2("-- ipbuf=%s, remote_ip=%s",ipbuf,remote_ip);
                            safeStrcpy(ipbuf,remote_ip,ipbuf_size-1);
                        }
                    }
                    else
                    {
                        Debug2("ipbuf is NULL",0,0);
                        /*
                        ** remote IP is not NULL, and ipbuf is NULL, that means
                        ** the datafile has only digit part in it. overwrite
                        ** ipbuf with remote_ip
                        */
                        safeStrcpy(ipbuf,remote_ip,ipbuf_size-1);
                        
                    }
                }
                else
                {
                    Debug2("remote_ip is NULL, it should not be!",0,0);
                }
            }
        }

    }

        /*
        ** if the content of the dbuf has anything but digit, replace
        ** the content with 1. If the content has digits and non-digits,
        ** null terminate at the first non-digit character
        */
        cleanBuf(dbuf,n,&length);

        if ((*dbuf == '\0') || (strcmp(dbuf,"0") == 0))
        {
            /* for bc lib */
            (void) strcpy(dbuf,"1");
            length=1;
        }
        else
        {
            /*
            ** now add 1 to the buffer. note: there's no fear to overflow 
            ** int buffer. We can have digits as big as MAX_DIGITS long.
            ** the big number adding routines are from GNU bc library
            */

            if ((do_increment == True) && (ignore_site == False))
            {
                /*
                ** use bigplus lib from GNU bc to add 1. it can do 
                ** arbitrary length arith.
                */
                add1toData(dbuf,malloc_size-1);

            }
        }
        if (ignore_site == False)
        {
           /* overwrite readbuf, we don't need it anymore */
            memset(readbuf,0,malloc_size);
            safeStrcpy(readbuf,dbuf,malloc_size-1);
            /* 
            ** if reload ignore is false, then don't bother to 
            ** add the IP ad the end
            */
            if (getcfgCountReload() == False)   /* dont count reload */
            {
                /* at this point, ipbuf has something in it, is it?*/
                if ((ipbuf != (char *) NULL) && (*ipbuf != '\0'))
                {
                    Debug2("ipbuf is not NULL=%s",ipbuf,0);
                    safeStrcat(readbuf,":",1,malloc_size-1,strlen(readbuf));
                    safeStrcat(readbuf,ipbuf,IP_LENGTH,malloc_size-1,
                        strlen(readbuf));
                }
            }
            else
            {
                /*
                ** site admin decided to count reload. now users have the option
                ** to increment or not
                */
                if (getUserReload() == False)
                {
                    /* at this point, ipbuf has something in it, is it?*/
                    if ((ipbuf != (char *) NULL) && (*ipbuf != '\0'))
                    {
                        Debug2("ipbuf is not NULL=%s",ipbuf,0);
                        safeStrcat(readbuf,":",1,malloc_size-1,strlen(readbuf));
                        safeStrcat(readbuf,ipbuf,IP_LENGTH,malloc_size-1,
                        strlen(readbuf));
                    }
                }
            }
        }
    }
    else    /* nothing was read */
    {
        if (use_st == True)
        {
            if (start_value <= 0)
                start_value=1;
            (void) sprintf(dbuf,"%d",start_value);
        }
        else
        {
            (void) strcpy(dbuf,"1");
        }

        /* create readbuf for writing */
         memset(readbuf,0,malloc_size);
         safeStrcpy(readbuf,dbuf,malloc_size-1);

        /* 
        ** if reload ignore is false, then don't bother to add the IP 
        ** at the end
        */
        if (getcfgCountReload() == False)   /* dont Count reload */
        {
            if (remote_ip != (char *) NULL)
            {
                safeStrcat(readbuf,":",1,
                        malloc_size-1,strlen(readbuf));
                safeStrcat(readbuf,remote_ip,IP_LENGTH,
                        malloc_size-1,strlen(readbuf));
            }
        }
        else
        {
            /*
            ** site admin decided to count reload. now users have the option
            ** to increment or not
            */
            if (getUserReload() == False)
            {
                if (remote_ip != (char *) NULL)
                {
                    safeStrcat(readbuf,":",1,
                        malloc_size-1,strlen(readbuf));
                    safeStrcat(readbuf,remote_ip,IP_LENGTH,
                        malloc_size-1,strlen(readbuf));
                }
            }
        }
   }

   if (ignore_site == False)
   {
        (void) lseek(fd,0L,0);
        Debug2(" -- readbuf=%s",readbuf,0);

        /* Note: ftruncate() is gone! */

        /* at his point, readbuf has something in it */
        /* and it must be NULL terminaed as well */
        if (strlen(readbuf) > 0)
        {
            (void) strcat(readbuf,"$");
            (void) write(fd,readbuf,strlen(readbuf));
        }
    }
 
    (void) close(fd);   /* unlocks as well */
    
#ifdef SYS_UNIX
        UnsetLock(fd);
#endif

    if (readbuf != (char *) NULL)
        (void) free(readbuf);
    if (ipbuf != (char *) NULL)
        (void) free (ipbuf);

    return (dbuf);
}


/*
** add 1 to a very large number (80 digit long max in our case)
** uses the bigplus (I named it) library from GNU bc 
*/

/* buf is changed */
static void add1toData(buf,buflen)
char
    *buf;
int
    buflen;
{
    bc_num
        n1,
        n2,
        result;

    char
        tbuf[2];

    int
        scale=0;

    Debug2("buf in add1toData()=%s",buf,0);
    (void) strcpy(tbuf,"1");

    init_numbers();
    init_num(&n1);
    init_num(&n2);
    init_num(&result);


    str2num(&n1,buf,scale);
    str2num(&n2,tbuf,scale);

    bc_add(n1,n2,&result,scale);
    safeStrcpy(buf,num2str(result),buflen);

    free_num(&n1);
    free_num(&n2);
    free_num(&result);
}

/*
**  getParts()
**  get the digit and IP part out of a buffer, the items are separated
**  by a :
**
**  RCS
**      $Revision: 1.1.1.1 $
**      $Date: 2001/03/19 01:59:51 $
**  Return Values:
**      none
**
**  Parameters:
**      allbuf  digit+IP
**      dbuf    will contain digit part (returns)
**      ipbuf   will contain 15 character IP    (returns)
**
**  Side Effects:
**      contents of all buffers are changed
**
**  Limitations and Comments:
**      all the buffer must have space allocated for them before passing
**      then here. The caller must pass the correct size of the buffers
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-24-1997    first cut
*/

static void getParts(allbuf,dbuf,ipbuf,dbuf_size,ipbuf_size)
char
    *allbuf,
    *dbuf,
    *ipbuf;
int
    dbuf_size,
    ipbuf_size;
{
    char
        *p,
        *v;

    p=allbuf;
    v=mystrtok(p,":");
    if (v != (char *) NULL)
    {
        safeStrcpy(dbuf,v,dbuf_size);
        /*
        Debug2("in getParts() dbuf=%s,strlen(dbuf)=%d",dbuf,strlen(dbuf));
        */

        p=(char *) NULL;
        v=mystrtok(p,":");
        if (v != (char *) NULL)
        {
            safeStrcpy(ipbuf,v,ipbuf_size);
            for (p=ipbuf; *p != '\0'; p++)
            {
                if (*p == '$')
                {
                    *p='\0';
                    break;
                }
            }
        }
    }
}

