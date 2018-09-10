/* utility routines for the coutner */

#include "cdebug.h"

#include "combine.h"
#include "gdfonts.h"
#include "count.h"


/*
 *  USE_DNS added by Sean Reifschneider <jafo@tummy.com> 1997-11-21.
 *
 *  When defined, USE_DNS will cause the referer host to be looked up
 *  in DNS for checking in the .cfg file.  This allows people running
 *  virtual hosts to list only their IP addresses instead of having to
 *  add each virtual domain name to the .cfg file.
 */

#ifdef USE_DNS
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


#ifdef SYS_WIN32
#include "configNT.h"
#elif defined( __VMS )
#include "configVMS.h"
#else
#include "config.h"
#endif

#ifdef __VMS
#define DIR_SEPARATOR "" /* Already have ] at end of _DIR macros */
/* VMS DIGIT_DIR ends in . so need to add subdirectory and ] - handled later */
#define SYM_PREFIX "WWW_"
#else
#define DIR_SEPARATOR "/"
#define SYM_PREFIX ""
#endif

static char *month_name[]=
{
    "Jan", "Feb", "Mar", "Apr",
    "May", "Jun", "Jul", "August",
    "Sep", "Octr","Nov", "Dec"
};


static int howmanyCommas _Declare((char *));
static unsigned long my_inet_addr _Declare ((char *));

/*
**  howmanyCommas()
**  calculates how many commas will be added to the buffer
**
**  RCS
**      $Revision: 1.2 $
**      $Date: 2001/04/08 16:15:51 $
**  Return Values:
**      no of commas 
**
**  Parameters:
**      buf     buf to scan
**
**  Side Effects:
**      none
**
**  Limitations and Comments:
**      buf must be a initialized string (at least)
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-18-1997    first cut
*/
static int howmanyCommas(buf)
char
    *buf;
{
    int
        n=0,
        length;

    if (*buf == '\0')
        return(0);

    length=strlen(buf);

    if ((length % 3) == 0)
        n=(length/3)-1;
    else
        n=length/3;

    if ( n < 0)
        n=0;

    Debug2("no of commas=%d",n,0);
    return(n);
}


/*
**  Commaize_buggy()
**  add a comma after every 3rd digit from right
**
**  RCS
**      $Revision: 1.2 $
**      $Date: 2001/04/08 16:15:51 $
**  Return Values:
**      none
**
**  Parameters:
**      buf     buf to modify
**
**  Side Effects:
**      buf is modified if it is atleast 4 character long
**
**  Limitations and Comments:
************************************************************************
**     ********** Don't use this, it's a bug!, mm, may-26-1999 *****
************************************************************************
**
**  Development History:
**      who                  when       why
**      ma_muquit@fccc.edu   no idea    first cut
**                           Oct-18-97  added dynamic buffer 
*/


void Commaize_buggy(buf)
char
    *buf;
{
    char
        *pbuf=(char *) NULL;

    register char
        *p,
        *q;

    int
        ncommas=0,
        m=0,
        i=-1;


    if (*buf != '\0')
    {
        ncommas=howmanyCommas(buf);
        /*
        ** patch sent by James Burton <jamesb@aone.com.au>
        ** Oct-21-1997
        */
        if (ncommas == 0)
            return;

        if (ncommas > 0) /* atleat 4 characters */
        {
            pbuf=(char *) malloc(strlen(buf)+ncommas*sizeof(char)+1);
            if (pbuf == (char *) NULL)
            {
                /* return quietly */
                return;
            }
        }
        m=0;
        m=m+strlen(buf)/3;
        if (strlen(buf)%3 == 0)
            m--;

        p=buf+strlen(buf)-1;
        pbuf[strlen(buf)+m]='\0';
        q=pbuf+strlen(buf)+m-1;
        while (1)
        {
            if (p < buf)
                break;
            if (++i == 3)
                i=0, *q-- = ',';
             *q-- = *p--;
        }
        (void) strcpy(buf,pbuf);
        if (pbuf)
            (void) free(pbuf);
    }

}


/*
**  Commaize()
**  add a comma after every 3rd digit from right
**
**  RCS
**      $Revision: 1.2 $
**      $Date: 2001/04/08 16:15:51 $
**  Return Values:
**      none
**
**  Parameters:
**      buf     buf to modify
**
**  Side Effects:
**      buf is modified
**
**  Limitations and Comments:
**      buf must have enough space to hold the extra , characters
**
**  Development History:
**      who                  when       why
**      ma_muquit@fccc.edu   no idea    first cut
**                           Oct-18-97  added dynamic buffer 
**                           May-26-1999 rewrote as the old version has
**                           memory overstepping bug. Code adapted from
**                           C-Snippets, file commaflt.c by Bruce Wedding and
**                           and Kurt Kuzba
*/

void Commaize(buf)
char
    *buf;
{
    char
        *pbuf=(char *) NULL;

    int
        bf=0,
        cm=0,
        tm=0;

    if (*buf != '\0')
    {
        pbuf=mystrdup(buf);
        if (pbuf == (char *) NULL)
            return; /* malloc failed, return quitely */

        reverseString(pbuf);
        while ((buf[bf++]=pbuf[tm++]) != 0)
        {
            if(++cm % 3 == 0 && pbuf[tm])
                buf[bf++]=',';
        }
        if (pbuf)
            (void) free((char *) pbuf);

        reverseString(buf);
    }
}



/*
** print the Content-type MIME header
*/
void PrintHeader ()
{

    char
        expires[64],
        date[64];

    int
        exp;
    /* check if caching should be disabled */
    exp=getExpireOffset();

    Debug2("expires=%d",exp,0);
    if (exp >= 0)
    {
        Debug2("Expire in: %d seconds",exp,0);
        getHTTPexpires(exp,expires,date,63,sizeof(expires)-1,sizeof(date)-1);
        Debug2("expires=%s",expires,0);
        Debug2("date=%s",date,0);
        if (Gdebug == False)
        {
/* Using DECthreads OSU server under VMS it allows more flexibility */
/* if we set expiry time in COUNT.COM, e.g. we can use DECnet record mode */
#ifndef __VMS
            (void) fprintf(stdout,"%s%c",expires,LF);
            (void) fprintf(stdout,"%s%c",date,LF);
#endif
        }
    }

    if (Gdebug == False)
    {
/* Using DECthreads OSU server under VMS it allows more flexibility */
/* if we send the content type ourselves, e.g. we can use DECnet record mode */
#ifndef __VMS
        (void) fprintf (stdout,"Content-type: image/gif%c%c",LF,LF);
        (void) fflush (stdout);
#endif
    }
    return;
}

/*
** check if the counter file exists
*/

int CheckFile (filename)
char
    *filename;
{
    int
        rc=0;

    rc = access (filename, F_OK);
    return rc;
}

/*
 * checkfilename:
 * - check to see if a path was specified - return 1 if so, 0 otherwise
 * it might not be foolproof, but I can't come up with
 * any other ways to specify paths.
 * by carsten@group.com (07/27/95)
 * ..\filename was not getting ignored on NT
 * reported by asolberg@pa.net>
 * fixed: 04/19/96
 */

int checkfilename(str)
char
    *str;
{
    if (strcmp(str,"..") == 0)
    {
        return 1;
    }

    while (*str)
    {
        if ((*str == '/') || 
            (*str == '\\') || 
            (*str == ':') ||
            (*str == '~'))
            return 1;
        str ++;
    }
    return 0;
}

/*
**  cMalloc()
**  a wrapper around malloc
**
**  RCS
**      $Revision: 1.2 $
**      $Date: 2001/04/08 16:15:51 $
**  Return Values:
**      returns a pointer to allocated space
**
**  Parameters:
**      size    bytes to malloc
**
**  Side Effects:
**      space is allocated
**
**  Limitations and Comments:
**      will write the error message image and exit
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-12-1997    first cut
*/

char *cMalloc(size)
int
    size;
{
    char
        *ptr;

    ptr=(char *) malloc(size*sizeof(char));

    if (ptr == NULL)
    {
        StringImage("Memory allocation problem!");
        exit(0);
    }

    return(ptr);
}


/*
** NULL terminate the buffer at the first sight of a non-digit character
*/
void cleanBuf(buf,bytes_in_buf,length)
char
    *buf;
int
    bytes_in_buf;
int
    *length;
{
    int
        i;


    int
        c=0;
    for (i=0; i < bytes_in_buf; i++)
    {
        if (!isdigit(buf[i]))
        {
            buf[i]='\0';
            break;
        }
        c++;
    }

    *length=c;
}
/*
**  safeStrcpy()
**  copy a string to another safely without overflowing buffer
**
**  RCS
**      $Revision: 1.2 $
**      $Date: 2001/04/08 16:15:51 $
**  Return Values:
**      none
**
**  Parameters:
**      to_str      destination buffer
**      from_str    source buffer
**      length      max allowable length of the from_str
**
**  Side Effects:
**      if copying is safe, to_str buffer is modified.
**
**  Limitations and Comments:
**      if the source string is longer than length, it's truncated to
**      length. to_str must be static or dynamically allocated buffer, that
**      is it must be able to hold atleast lenth bytes buffer. The from_str
**      is checked if it is NULL or not. if NULL the routine will exit 
**      after writing an error message image.
**      
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-17-1997    first cut
*/

void safeStrcpy(to_str,from_str,length)
char
    *to_str,
    *from_str;
int
    length;
{
    /*
    ** let turn off the followin message, I think we tested it enough :)
    */
    /*
    if ( NULL == from_str )
    {
        StringImage("Source buffer is NULL in safeStrcpy()!");
        exit(0);
    }
    */

    /*
    ** if source string is longer than to buffer, it will be 
    ** truncated anyway, so let get rid of the buffer overflow message
    */
    /*
    if (strlen(from_str) > length)
    {
        PrintHeader();
        StringImage("buffer overflow detected! aborting");
        exit(0);
    }
    */

    /* now copy */
    (void) strncpy(to_str,from_str,length);
    to_str[length] = '\0';
}

/*
 *  safeStrcat()
 *  safely appends one string to another without overflowing the source
 *  buffer.
 *
 *  Parameters:
 *      to_str          appends with this string
 *      str             string to append
 *      length          first this many character of str to append
 *      ssc_size        maximum size of destination string
 *      ssc_length      length of string in destination buffer
 *
 *  Return Values:
 *      none
 *
 *  Limitations and Comments:
 *      to_str must have enough space staically or dynamically allocated
 *      before calling. Need to provide a portable strncat() as matt
 *      suggested that some versions of strncat (e.g. solaris 2.5.1) write
 *      at most n+1 characters past the end of s1 and then replace the 
 *      n+1st character with a '\0';
 *
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Oct-1997       first cut
 *      mhpower@mit.edu      Nov-03-1997    fixed
 */


void safeStrcat(to_str,str,length,ssc_size,ssc_length)
char *to_str,
     *str;
int
    length,
    ssc_size, 
    ssc_length;
{
   int copy_length;

   if ( NULL == str )
   {
       StringImage("Source buffer is NULL in safeStrcat()!");
       exit(0);
   }

   if (strlen(str) >= ssc_size - ssc_length)
   {
       StringImage("buffer overflow detected! aborting");
       exit(0);
   }

   if (length < ssc_size - ssc_length)
   {
       copy_length = length;
   }
   else
   {
       copy_length = ssc_size - ssc_length;
   }

   /* now copy */
   (void) mystrncat(to_str,str,copy_length);

   /*
   to_str[ssc_length + copy_length] = '\0';
   */
}



/* case insensitive version of ANSI strstr() */
/* from swish package by kevin h , kevin called it lstrstr()*/
/*
** s - hay stack
** t - needle
*/
char *mystristr(s,t)
char
    *s;
char
    *t;
{

int
    i,
    j,
    k,
    l;

    for (i = 0; s[i]; i++) 
    {
        for (j = 0, l = k = i; s[k] && t[j] &&
            tolower(s[k]) == tolower(t[j]); j++, k++)
            ;
        if (t[j] == '\0')
            return s + l;
    }
    return NULL;
}

/*
**  authRemhost()
**  authenticare remote host
**
**  RCS
**      $Revision: 1.2 $
**      $Date: 2001/04/08 16:15:51 $
**  Return Values:
**      if HTTP_REFERER env variable exists:
**        returns (1) only if host is authorized. othewise writes the error
**        message image and exits.
**      if HTTP_REFERER env variabel does not exist
**          return 0
**
**  Parameters:
**      none
**
**  Side Effects:
**      none
**
**  Limitations and Comments:
**      none
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-18-1997    first cut
*/

int authRemhost()
{
    int
        rc=0;

    char
        *prem_refh=(char *) NULL,
        szbuf[BUFSIZ],
        szHttp_ref[1024],
        szRhost[1024];

#ifdef USE_DNS
    struct hostent
        *he;
    char
        *revname,
        *ipaddr;
#endif

    *szHttp_ref='\0';
    *szRhost='\0';

#ifdef __VMS
    prem_refh=(char *) getenv(SYM_PREFIX "HTTP_REFERER");
#else
    prem_refh=(char *) getenv("HTTP_REFERER");
#endif

    if (prem_refh != (char *) NULL)
    {
        safeStrcpy(szHttp_ref,prem_refh,sizeof(szHttp_ref)-1);
        GetRemoteReferer(szHttp_ref,szRhost);

        if (*szRhost != '\0')
        {
#ifdef USE_DNS
            /*  get resolved host name and ip address as well  */
             revname = NULL;
             ipaddr = NULL;
             if ((he = gethostbyname(szRhost))) 
             {
                revname = he->h_name;
                ipaddr = inet_ntoa(*(struct in_addr *) he->h_addr);
             }
#endif

            rc=hostAuthorized(szRhost);
#ifdef USE_DNS
            if (!rc && ipaddr)
            {
                rc=hostAuthorized(ipaddr);
            }
#endif /* USE_DNS */
            if (rc == True)     /* Authorized */
                return (True);
            
            if (rc == False)
            {
                *szHttp_ref='\0';
                if (strlen(szRhost) + 27 > sizeof(szHttp_ref))
                {
                    (void) sprintf(szHttp_ref,
                                "Host: \"(name too long)\" is not authorized");
                }
                else
                {
                    (void) sprintf(szHttp_ref,"Host: \"%s\" is not authorized",
                                   szRhost);
                    Warning(szHttp_ref);
                    StringImage(szHttp_ref);

                    /*
                    ** log the offending page too. Oops, the code was
                    ** buggy! may be I was sleepy? Someone sent the fix.
                    ** will put the name in ChangeLog.
                    ** Sep-07-2000
                    */
                    if (strlen(prem_refh) + 16 < sizeof(szbuf))
                    {
                        (void) sprintf(szbuf,"Referer page: %s\n",
                                       prem_refh);
                        Warning(szbuf);
                    }
                }

                exit(0);
            }
        }
    }
    else
        rc=0;

    return (rc);
}


/*
**  isinname()
**  checks if the mask fits in the string
**
**  RCS
**      $Revision: 1.2 $
**      $Date: 2001/04/08 16:15:51 $
**  Return Values:
**      1       if fits
**      0       if not
**
**  Parameters:
**      string      source string
**      mask        mask string
**
**  Example:
**      www.fccc.edu  *.fccc.edu        fits
**      www.fccc.edu  www*              fits
**      www.fccc.edu  *fccc*            fits
**      132.138.4.6   132*              fits
**      etc.....
**
**  Side Effects:
**      none
**
**  Limitations and Comments:
**      borrowed from swish by Kevin H. it's buggy. use wildMat() instead
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-18-1997    first cut
*/

int isinname(string, mask)
char
    *string;
char
    *mask;
{
    int
        i,
        j;

    char
        firstchar,
        lastchar,
        *tempmask;

    if ((*string == '\0') || (*mask == '\0'))
        return (0); /* mm */

    if (!strcmp(mask, "*"))
            return 1;

    firstchar=mask[0];
    lastchar=mask[(strlen(mask) - 1)];
    tempmask=(char *) cMalloc(strlen(mask)+1);

    for (i = j = 0; mask[i]; i++)
        if (mask[i] != '*')
            tempmask[j++] = mask[i];

    tempmask[j]='\0';
    if (firstchar == '*')
    {
        if (lastchar == '*')
        {
            if ((char *) mystristr(string, tempmask))
            {
                free(tempmask);
                return 1;
            }
        }
        else
        {
            if ((char *) mystristr(string, tempmask) ==
                 string + strlen(string) - strlen(tempmask))
            {
                free(tempmask);
                return 1;
            }
        }
    }
    else if (lastchar == '*')
    {
        if ((char *) mystristr(string, tempmask) == string)
        {
            free(tempmask);
            return 1;
        }
    }
    else
    {
        /*
        ** changed from strcmp(), patch sent my
        ** Takeshi OKURA <okura@osa.ncs.co.jp>
        ** Oct-30-1997
        */
        if (!mystrcasecmp(string, tempmask))
        {
            free(tempmask);
            return 1;
        }
    }
    free(tempmask);

    return 0;
}





/*
** get current time
*/

char *GetTime ()
{
    time_t
        tm;

    char
        *times;

    tm = time (NULL);
    times = ctime(&tm);
    times[(int) strlen(times)-1] = '\0';
    return (times);
}

void Warning (message)
char
    *message;
{
    char
        *times;
    FILE
        *fp= (FILE *) NULL;
    char
        buf[1024];

    /*
    ** if configured not to log messages, just return
    */
    if (getcfgLogErrmsg() == False)
        return;

    *buf='\0';

#ifdef SYS_WIN32
    (void) sprintf(buf,"%s%s/%s",g_sZ_vbase,LOG_DIR,LOG_FILE);
#else
    (void) sprintf(buf,"%s" DIR_SEPARATOR "%s",LOG_DIR,LOG_FILE);
#endif

    times=GetTime();


    fp = fopen(buf, "a");
    if (fp == (FILE *) NULL)
    {
        /*
        ** netscape web server merges stderr and stdout together, so if you'r
        ** using netscape server, you're busted, you not going to know why
        ** the program is not working. If you use apache, you will see error
        ** messages in the server error log
        ** --play safe, just return quietly (mm, jan-21-1999)
        */
/*
(void) fprintf (stderr,"[%s] Count %s: Could not open CountLog file %s\n",
                times,Version,buf);
*/
        return;
    }
    (void) fprintf (fp,"[%s] Count %s: %s\n", times, Version,message);

    if (fp != stderr)
        (void) fclose (fp);
}



/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
** I'm renaming it to mystrtok() in order to avoid conflict with the
** system which might have it
** I also formatted to my coding style
** 10/08/95, muquit@semcor.com
*/

char *mystrtok(s, delim)
char
    *s;
char
    *delim;
{
	register char
        *spanp;

	register int
        c,
        sc;

	char
        *tok;

	static char
        *last;


	if (s == (char *) NULL && (s = last) == (char *) NULL)
		return ((char *) NULL);

	 /*
	 ** Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) 
    {
		if (c == sc)
			goto cont;
	}

	if (c == 0) 
    {		/* no non-delimiter characters */
		last = (char *) NULL;
		return ((char *)NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */

	for (;;) 
    {
		c = *s++;
		spanp = (char *)delim;
		do 
        {
			if ((sc = *spanp++) == c) 
            {
				if (c == 0)
					s = (char *) NULL;
				else
					s[-1] = '\0';
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	  /* NOTREACHED */
}

/*
** muquit, first cut: Jan-21-1999
** was ASI definition. fixed. apr-26-1999
*/
int mystrncasecmp(s1,s2,n)
char
    *s1,
    *s2;
int
    n;
{
    register char
        *scan1,
        *scan2;

    int
        count;

    scan1=s1;
    scan2=s2;
    count=n;

    while (--count >= 0 && *scan1 != '\0' && tolower(*scan1) == tolower(*scan2))
    {
        scan1++;
        scan2++;
    }
    if (count < 0)
        return (0);


    return(tolower(*scan1) - tolower(*scan2));
}


/*
** found somewhere in the net
** muquit@semcor.com
*/
int mystrcasecmp(a,b)
char
    *a,
    *b;
{	
    register char
        ac,
        bc;

	for(;;)
    {
		ac = *a++;
		bc = *b++;

		if(ac == 0)
        {
			if(bc == 0)
				return 0;
			else
                return -1;
        }
		else
        { 
            if(bc == 0)
				return 1;
			else 
            {
                if(ac != bc)
                {
					if(islower(ac)) ac = toupper(ac);
					if(islower(bc)) bc = toupper(bc);
					if( ac != bc )
						return ac - bc;
				}
            }
	    }
    }
}


int CheckDirs()
{
    if ((strcmp(CONFIG_DIR,DATA_DIR) == 0) ||
        (strcmp(CONFIG_DIR,DATA_DIR) == 0) ||
        (strcmp(CONFIG_DIR,LOG_DIR) == 0) ||
        (strcmp(DIGIT_DIR,DATA_DIR) == 0) ||
        (strcmp(DIGIT_DIR,LOG_DIR) == 0) ||
        (strcmp(DATA_DIR,LOG_DIR) == 0))
    return (1);
return (0);
}



/*
 * the different -- and hopefully self-contained -- version of
 * CheckRemoteIP. Main difference is that ignore host block may
 * contain two IP address fields. First is then IP network address
 * and second is the netmask to be applied for comparision.  
 *
 * Davorin Bengez (Davorin.Bengez@science.hr), Jan 11. 1996.
 */

int CheckRemoteIP(remote_ip,ip_ign)
char *remote_ip, *ip_ign;
{
    unsigned long
        rem_ip,
        ignore_ip,
        ignore_mask;

    char
        addr_buf[20],
        mask_buf[20];

    int
        rc=False,
        i;

    ignore_mask = 0xffffffffL;

    rem_ip = my_inet_addr(remote_ip);
    if(rem_ip == (unsigned long)-1) return(False);

    /*
     * if ip_ign has TWO ip-look-alike fields, second field is netmask
     */

    if((i = sscanf(ip_ign, "%s %s", addr_buf, mask_buf)) < 1)
    {
        return(False);
    }
    ignore_ip = my_inet_addr(addr_buf);
    if (ignore_ip == (unsigned long)-1)
        return(False);

    /*
     * try to convert the mask to something usable and fail if it
     * is not the proper IP netmask...
     */

    if(i == 2)  /* with netmask */
    {
        Debug2("in netmask ==> ignore IP=%s",ip_ign,0);
        ignore_mask = my_inet_addr(mask_buf);
        if(ignore_mask == (unsigned long)-1)
            return(False);

        /*
         * ...and finally, compare the masked addresses...
         */
  
        if((rem_ip & ignore_mask) == (ignore_ip & ignore_mask))
            return(True);
        else
            return(False);
    }
    else if (i == 1)  /* a single IP or wild card */
    {
        Debug2("in sing/wildcard ==> ignore IP=%s",ip_ign,0);
        /* it's buggy.. muquit@muquit.com Sun Mar 18 21:47:50 EST 2001 */
        /*
        rc=isinname(remote_ip,ip_ign);
        */

        rc=wildMat(remote_ip,ip_ign);
        Debug2("ignore %s=%d",remote_ip,rc);
        return (rc);
    }

    return (False);
}

/*
 * and a version of inet_addr - not to link all the network services
 * just because of this one...
 */

static unsigned long my_inet_addr(s)
char *s;
{
    unsigned long n;
    int i;

    n = 0;

    for(i = 24; i >= 0; i -= 8) {
        n |= (unsigned long)atoi(s) << i;
        if((s = strchr(s,'.')) == (char *)NULL)
            break;
        ++s;
    }
    return(n);
}
/*
**  getdateGMT()
**  get the today's date in GMT
**
**  RCS
**      $Revision: 1.2 $
**      $Date: 2001/04/08 16:15:51 $
**  Return Values:
**      pointer to a static string buffer of asctime. 
**
**  Parameters:
**      none
**
**  Side Effects:
**      pointer may get squashed with the next call to asctime. so
**      store it yourself if needed.
**
**  Limitations and Comments:
**      some wise guy decided to add a newline at the end of the string
**      returned by asctime. the new line is removed.
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-25-1997    first cut
*/

char *getdateGMT()
{
    time_t
        t_time_t;

    struct tm
        *p_tm;

    char
        *tmp,
        *ptr;

    t_time_t=time(NULL);
    p_tm=gmtime(&t_time_t);
    ptr=asctime(p_tm);

    if (ptr && ((tmp=strchr(ptr,'\n')) != NULL))
        *tmp='\0';

    return(ptr);
}

/*
**  getHTTPexpires()
**  gets HTTP Expires and Date header
**
**  RCS
**      $Revision: 1.2 $
**      $Date: 2001/04/08 16:15:51 $
**  Return Values:
**      none
**
**  Parameters:
**      offset      offset in seconds from current time 
**                  create the expire header after adding this seconds offset
**                  to current time.
**      expires     Expire header (in asctime format in GMT), returns
**      date        Date (asctiem format in GMT), returns
**      length      this many chacters to copy to expires or date buffer
**      exp_size    size of expires buffer
**      date_size   size of date buffer
**
**  Side Effects:
**      the contents of expires and date are changed. the caller is 
**      reponsible to call with corrent size.
**
**  Limitations and Comments:
**      expires and date must have enough space to hold the Expires and
**      Date strings. the caller is responsible to pass the corrent
**      size length.
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-25-1997    first cut
*/

void getHTTPexpires(offset,expires,date,length,exp_size,date_size)
int
    offset;
char
    *expires,
    *date;
int
    length,
    exp_size,
    date_size;

{
    time_t
        t_time_t;

    struct tm
        *p_tm;

    char
        *tmp,
        *ptr;

    *expires='\0';
    *date='\0';

    if (offset < 0)
        offset=0;

    t_time_t=time(NULL);
    p_tm=gmtime(&t_time_t);
    ptr=mystrdup(asctime(p_tm));
    if (ptr && ((tmp=strchr(ptr,'\n')) != NULL))
        *tmp='\0';
    
    /*
    ** Date: header
    */
    safeStrcpy(date,"Date: ",length);
    safeStrcat(date,ptr,length,date_size,strlen(date));
    (void) free(ptr);

    t_time_t += offset;
    p_tm=gmtime(&t_time_t);
    ptr=mystrdup(asctime(p_tm));
    if (ptr && ((tmp=strchr(ptr,'\n')) != NULL))
        *tmp='\0';

    /*
    ** Expires: header
    */
    safeStrcpy(expires,"Expires: ",length);
    safeStrcat(expires,ptr,length,exp_size,strlen(expires));
    (void) free(ptr);
}

/*
** duplicate a string
*/
char *mystrdup (string)
char
    *string;
{
    char
        *tmp;

    if (string == (char *) NULL)
        return ((char *) NULL);

    /*
    **  malloc twice as much memory as a work around for something elsewhere 
    **  muquit, may-26-1999
    */
    
    tmp = (char *) malloc ((int) strlen(string)*2 );

    if (tmp == (char *) NULL)
    {
        StringImage("memory allocation problem mystrdup()");
        exit(0);
        return ((char *) NULL);
    }
    /* it's safe to copy this way */
    (void) strcpy(tmp, string);
    return (tmp);
}



/*
** from wusage 2.3
*/
void RemoveTrailingSp (string)
char
    *string;
{
    while (True)
    {
        int
            l;

        l = (int) strlen (string);
        if (!l)
            return;

        if (string[l-1] == ' ')
            string[l-1] = '\0';
        else
            return;
    }
}
/*
 *  mystrncat()
 *  appends at most n characters of src string to dst string
 *
 *  Parameters:
 *      dst     destination string
 *      src     source string
 *      n       number of character to take from src
 *
 *  Return Values:
 *      pointer to dst string
 *
 *  Limitations and Comments:
 *      taken from Henry Spencer's public domain string library
 *
 *
 *  Development History:
 *      who                  when           why
 *      ma_muquit@fccc.edu   Nov-03-1997    first cut
 */

char *mystrncat(dst,src,n)
char
    *dst,
    *src;
int
    n;
{
    register char
        *dscan,
        *sscan;

    register int
        count;

    for (dscan=dst; *dscan != '\0'; dscan++)
        continue;
    
    sscan=src;
    count=n;

    while (*sscan != '\0' && --count >= 0)
        *dscan++ = *sscan++;

    *dscan++ = '\0';

    return (dst);
}

/*
 * strncpy - copy at most n characters of string src to dst
 * adapted from henry's stringlib
 */
char *              /* dst */
mystrncpy(dst,src,n)
char
    *dst,
    *src;
int
    n;
{
    register char
        *dscan;
    register char
        *sscan;
    register int
        count;

    dscan = dst;
    sscan = src;
    count = n;
    while (--count >= 0 && (*dscan++ = *sscan++) != '\0')
        continue;
    while (--count >= 0)
        *dscan++ = '\0';
    return(dst);
}




/*
**  allocCountdown()
**  allocate memory for the Countdown struct and initializes the members
**
**  Parameters:
**      none
**
**  Return Values:
**      Countdown   *       if succeeds
**      NULL                if fails
**
**  Limitations and Comments:
**      none
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   May-10-1998    first cut
*/

Countdown *allocCountdown()
{
    Countdown
        *cd;

    cd=(Countdown *) malloc(sizeof(Countdown));
    if (cd == (Countdown *) NULL)
        return ((Countdown *) NULL);

    cd->days=0;
    cd->hours=0;
    cd->minutes=0;
    cd->seconds=0;

    return (cd);
}

/*
**  calculate countdown time from a specific time and returns a pointer
**  to the struct Countdown
**
**  Parameters:
**      time_t  from_sec    From time in seconds
**
**  Return Values:
**      pointer to struct Countdown in success
**      NULL    in failure
**
**  Limitations and Comments:
**      none
**
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   May-10-1998    first cut for EJV
**                           Dec-13-1998    in Count 2.4
*/
Countdown *countDown(t_year,t_mon,t_day,t_hr,t_min,t_sec,t1)
int
    t_year,
    t_mon,
    t_day,
    t_hr,
    t_min,
    t_sec;

time_t
    t1;
{
    Countdown
        *cdown=(Countdown *) NULL;

    struct tm
        tm;

    time_t
        t2,
        tdiff;

    cdown=allocCountdown();
    if (cdown == NULL)
        return ((Countdown *) NULL);

    tm.tm_year=t_year-1900;        /* Year Since 1900 */
    tm.tm_mday=t_day;
    tm.tm_hour=t_hr;
    tm.tm_min=t_min;
    tm.tm_sec=t_sec;
    tm.tm_mon=t_mon-1;
    tm.tm_isdst=(-1);
    t2=mktime(&tm);

    tdiff=t2-t1;

    cdown->days=tdiff/(60*60*24);
    cdown->hours=(tdiff % (60*60*24))/3600;
    cdown->minutes=(tdiff % 3600)/60;
    cdown->seconds=(tdiff % 60);

    return (cdown);
}


/*
 *  checkTimezone - parses the GMT+-hhmm and returns the time in seconds
 *
 *  RCS:
 *      $Revision: 1.2 $
 *      $Date: 2001/04/08 16:15:51 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      This funcions parses a string like GMT+-hhmm and returns the time
 *      in seconds.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      char    str[]
 *          str holds a string like GMT+-hhmm
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Return Values:
 *      value   description
 *      int     diff
 *          hhmm is converted to seconds before returning. the value will
 *          be positive or negative based on GMT+ or GMT-
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      text
 *
 *  Development History:
 *      who                 when        why
 *      muquit@semcor.com   26-Mar-96   first cut
 */


long checkTimezone(str)
char
    str[];
{
    int
        hr,
        min;
    int
        d;
    int
        counter=0;

    char
        buf[10];

    long
        diff=0L;

    int
        i;

    *buf='\0';
    hr=0;
    min=0;
    

    if (*str != '\0')
    {
        if (strlen(str) == 8)
        {
            if ((str[3] == '-') || (str[3] == '+'))
            {
                (void) strcpy(buf,&str[4]);   
                for (i=0; i < strlen(buf); i++)
                {
                    d=buf[i];
                    if ((d >= '0') && (d <= '9'))
                    {
                        if (counter < 2)
                            hr=hr*10 +(d-'0');
                        else
                            min=min*10+(d-'0');
                        counter++;
                    }
                }
                diff=(hr*3600)+min*60;
                if (str[3] == '-')
                    diff = -(diff);
            }
        }
    }
    return (diff);
}
/*
**  getLocalTime()
**  calculate time like: Mon-Day-Year hr:min:sec AM/PM GMT+xyzw
**  Parameters:
**  DigitInfo   *digitinfo  
**  int add_from            if 1, add From: before the time string
**
**  Return Values:
**  char *time  on success
**  NULL on failure
**
**  Limitations and Comments:
**  returns pointer to a malloc'd space, caller is responsible to free it.
**  If gmtime() is not available in the system, it will call **  localtime() 
**  instead of gmtime()
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Jan-24-1999    first cut
**                           Jul-17-1999    first arg is digit_info now
*/

char  *getLocalTime(digit_info,add_from)
DigitInfo
    *digit_info;
int
    add_from;
{
    char
        *timezone_str,
        *s=(char *) NULL;

    struct tm
        from_tm,
        *ptm;

    time_t
        now;

    long
        diff;

    timezone_str=digit_info->time_z;

    /* if from values specified, calculate time_t for them -- mm,jul17,99 */
    if (digit_info->f_year >  0 &&
        digit_info->f_mon  >  0 &&
        digit_info->f_day  >  0 &&
        digit_info->f_hr   >= 0 &&
        digit_info->f_min  >= 0 &&
        digit_info->f_sec  >= 0)
    {
        from_tm.tm_year=digit_info->f_year - 1900;
        from_tm.tm_mon=digit_info->f_mon - 1;
        from_tm.tm_mday=digit_info->f_day;
        from_tm.tm_hour=digit_info->f_hr;
        from_tm.tm_min=digit_info->f_min;
        from_tm.tm_sec=digit_info->f_sec;
        from_tm.tm_isdst=(-1);

        now=mktime(&from_tm);
    }
    else
    {
        /* local time */
        now=time(NULL);
    }

    if (timezone_str == NULL || *timezone_str == '\0')
    {
        ptm=localtime(&now);
    }
    else
    {
        diff=checkTimezone(timezone_str);
        now += diff;
#ifdef HAVE_GMTIME
        ptm=gmtime(&now);
#else
        ptm=localtime(&now);
#endif /* HAVE_GMTIME */
    }
    s=cMalloc(512);
    if (add_from)
    {
        if (timezone_str == NULL || *timezone_str == '\0')
        {
            (void) sprintf(s,"%s %s %d %d, %d:%d:%d",
            "From:", month_name[ptm->tm_mon], ptm->tm_mday, 1900+ptm->tm_year,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        }
        else
            (void) sprintf(s,"%s %s %d %d, %d:%d:%d %s",
            "From:", month_name[ptm->tm_mon], ptm->tm_mday, 1900+ptm->tm_year,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec,timezone_str);
    }
    else
    {
        if (timezone_str == NULL || *timezone_str == '\0')
        {
            (void) sprintf(s,"%s %d %d, %d:%d:%d",
            month_name[ptm->tm_mon], ptm->tm_mday, 1900+ptm->tm_year,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        }
        else
        {
            (void) sprintf(s,"%s %d %d, %d:%d:%d %s",
            month_name[ptm->tm_mon], ptm->tm_mday, 1900+ptm->tm_year,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec,timezone_str);
        }
    }

    return (s);
}

void makeGIFPath(path,path_len,base,digit_info)
char
    *path;

int
    path_len;

char
    *base;

DigitInfo
    *digit_info;
{
            /* make the gif filepath*/
            safeStrcpy(path,base,path_len-1);
#ifndef __VMS
            safeStrcat(path,"/",path_len-1,path_len,strlen(path));
#endif

            safeStrcat(path,digit_info->ddhead,path_len-1,
                    path_len,strlen(path));
#ifndef __VMS
            safeStrcat(path,"/",path_len-1,path_len, strlen(path));
#else
/*          For VMS add a ] before adding name of .gif file  */
            safeStrcat(path,"]",path_len-1,path_len,
                    strlen(path));
#endif
            safeStrcat(path,digit_info->gif_file,path_len-1,
                    path_len,strlen(path));
}


/*
** borrowed from c-snippets STRREV.C, public domain by Bob Stout
** The name of the function was stttev()
** -- muquit, May-26-1999
** Comment: str is modified
*/
char *reverseString(str)
char
    *str;
{
    char
        *p1,
        *p2;

    if (! str || ! *str)
        return str;

    for (p1=str,p2 =str+strlen(str)-1; p2 > p1; ++p1, --p2)
    {
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return (str);
}

