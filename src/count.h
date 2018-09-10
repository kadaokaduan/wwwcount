/*
 *  Header file for Count
 *
 *  RCS:
*      $Revision: 1.2 $
 *      $Date: 2001/04/08 16:15:51 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      text
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Return Values:
 *      value   description
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      text
 *
 *  Development History:
 *      who                 when        why
 *      muquit@semcor.com   05-Jun-95   first cut
 */

#ifndef _COUNT_H
#define _COUNT_H

#include <stdio.h>

#if STDC_HEADERS || HAVE_STRING_H
#include <string.h> /* ANSI string.h and pre-ANSI memory.h might conflict*/
#if !STDC_HEADERS && HAVE_MEMORY_H
#include <memory.h>
#endif
#else
#if  HAVE_STRINGS_H
#include <strings.h>
#endif
#endif


#if HAVE_CTYPE_H
#include <ctype.h>
#endif

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#if SYS_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef SYS_WIN32
#include <io.h>
#include <share.h>
#define ftruncate chsize
#endif

#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#if HAVE_STDLIB_H 
#include <stdlib.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_FCNTL_H
#ifndef O_RDONLY    /* prevent multiple inclusion on lame systems (from vile)*/
#include <fcntl.h>
#endif
#endif

#if HAVE_MALLOC_H
#include <malloc.h>
#endif


#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif

#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif

#ifndef F_OK
#define F_OK    0
#endif

#ifndef True
#define True 1
#endif /* ! True */

#ifndef False
#define False 0
#endif /* ! False */

#ifndef Abort
#define Abort (-1)
#endif  /* ! Abort */

#define LF              10
#define SHOW_COUNTER    1
#define SHOW_CLOCK      2
#define SHOW_DATE       3
#define SHOW_GIF_FILE   4
#define SHOW_COUNTDOWN  5
#define SHOW_VERSION    6 

#define ALIGN_LEFT          1
#define ALIGN_CENTER        2
#define ALIGN_RIGHT         3 

#define ALIGN_TOP_LEFT      4
#define ALIGN_TOP_CENTER    5
#define ALIGN_TOP_RIGHT     6

#define ALIGN_MID_LEFT      7
#define ALIGN_MID_CENTER    8
#define ALIGN_MID_RIGHT     9

#define ALIGN_BOTTOM_LEFT   10
#define ALIGN_BOTTOM_CENTER 11
#define ALIGN_BOTTOM_RIGHT  12



#define DATE_MMDDYY     1
#define DATE_DDMMYY     2
#define DATE_YYMMDD     3
#define DATE_YYDDMM     4
#define DATE_MMYYDD     5
#define DATE_DDYYMM     6

#if __STDC__ || defined(sgi) || defined(_AIX)
#define _Declare(formal_parameters) formal_parameters
#else
#define _Declare(formal_parameters) ()
#define const
#endif

#ifdef Extern
#undef Extern
#endif

#ifndef __Main__
#define Extern extern
#else
#define Extern
#endif

#define Version "2.5"
#define Author          "muquit@muquit.com"
#define Url             "http://www.muquit.com/muquit/software/Count/Count.html"

#define IMAGE_COMMENT   "Count.cgi 2.5,(Apr-08-2001-1)\nBy Muhammad A Muquit\nhttp://www.muquit.com/muquit/software/Count/Count.html"

#define MaxTextLength 2048

/*
** maximum number of allowable digits, should be enough for you?
*/
#define MAX_DIGITS  80

/* to handle buffer overflow */
#define MAX_COUNTDOWN_YEAR      100000

/*
** ErrorCodes
*/
#define ConfigOpenFailed        100
#define NoIgnoreHostsBlock      101
#define UnpexpectedEof          102
#define NoRefhBlock             103
#define NoAccessList            104
#define IncompleteAccessList    105
#define NoAutofcBlock           106
#define NoStrictBlock           107
#define NoRgbMappingBlock       108
#define NO_RELOAD_BLOCK         109 

#define NoLoginName             200
#define NoDatafile              201

/*
** DigitInfo struct, moved out of libs/combine/combine.h
** Jan-24-1999
*/
typedef struct _DigitInfo
{
    char
        *pdigits;   /* digit strings */

    int
        maxdigits;

    unsigned int
        leftpad;

    unsigned int
        Frame;

    unsigned int
        alpha,
        width,
        height;

    unsigned int
        use_strip,
        comma;

    int
        cache,
        expires;

    int
        use_st;

    int
        nsegment;

    int
        replace_color;  /* internal flag */

    unsigned char
        alpha_red,
        alpha_green,
        alpha_blue;

    unsigned char   /* opaque color to replace */
        opaque_red,
        opaque_green,
        opaque_blue;

    unsigned char   /* pen color to replace a opaque color */
        pen_red,
        pen_green,
        pen_blue;

    char
        ddhead[100];
    char
        datafile[2048];

    char
        literal[2048];

    char
        gif_file[1024];     /* display this gif file */

    unsigned int
        st;

    unsigned int
        show;

    unsigned int
        increment_counter;

    unsigned int
        image_type; /* 1 GIF, 2 XBM for now */

    unsigned int
        negate;

    unsigned int
        rotate;

    unsigned int
        show_full_year;

    int
        rotate_degrees;

    int
        display_type;

    int
        date_format;

    int
        time_format;

    char
        time_z[50];

    int
        user_reload;

    /* xy location when image is composited with a base */
    int
        upl_x,
        upl_y;

    /* count down */
    int
        show_from_date;

    int
        t_year,
        t_mon,
        t_day,
        t_hr,
        t_min,
        t_sec;

    int
        f_year,
        f_mon,
        f_day,
        f_hr,
        f_min,
        f_sec;

    int
        show_dhms;  /* show day hr min sec in countdown or not */


    /* alignment on the base image */
    int
        align;  /* True of False */

    int
        alignment;  /* left, center or right */

    int
        offset;     /* offset of counter image from base image in pixels */

} DigitInfo;




/* 
** singly linked list struct
*/
typedef struct _Sllist
{
    char
        *item;
    struct _Sllist
        *next;
} Sllist;

/*
** Countdown structure, first cut: Dec-13-1998, Sunday
*/
typedef struct _Countdown
{
    int
        days,
        hours,
        minutes,
        seconds;
} Countdown;

/*
** struct to Hold the parameters of the count.cfg file
*/

typedef struct _cfg
{
    char
        *version;                           /* version string */
    int
        auto_file_creation;                 /* auto file creation, boolean */
    int
        strict_mode;                        /* boolean */
    int
        allow_rgb_database;                 /* bool */
    int
        count_reload;                       /* bool */
    int
        log_error_messages;                 /* bool */
    int
        show_error_messages_to_browsers;    /* bool */
    Sllist
        *ignore_ips;                        /* singly linked list */
    Sllist
        *auth_hosts;                        /* singly linked list */
} Cfg;



/*
** global variables
*/

    Extern unsigned int
        Gdebug;
    Extern int
        GrgbMappingIsError;
     
#ifdef SYS_WIN32
  /*
  ** path of the virtual base of the CGI program
  */
   Extern char
        g_sZ_vbase[256];
#endif  
    Extern char
        g_sZ_ibase[256];

/*
** maxumim line length in authorization file
*/
#define MaxLineLength 2048


#if MISSING_EXTERN_GETENV
extern char *getenv _Declare ((const char *));
#endif

#if MISSING_EXTERN_ATOI
extern int atoi _Declare ((char *));
#endif

#if MISSING_EXTERN_SRAND
extern void srand _Declare ((int));
#endif

#if MISSING_EXTERN_RAND
extern int rand _Declare ((void));
#endif

#if MISSING_EXTERN_ISDIGIT
extern int isdigit _Declare ((int));
#endif

#if MISSING_EXTERN_FLOCK
extern int flock _Declare ((int,int));
#endif

int         additemToList       _Declare ((Sllist **head,char *item));
Countdown   *allocCountdown     _Declare ((void));
void        assertSegarray      _Declare ((int idx,int max_idx,int c));
int         authRemhost         _Declare ((void));
Countdown   *countDown          _Declare ((int t_year,int t_mon,int t_day,
                                           int t_hr,int t_min,int t_sec,
                                           time_t t1));
int         checkfilename       _Declare ((char *));
long        checkTimezone       _Declare ((char *));
void        cleanBuf            _Declare ((char *buf,int bytes_in_buf,
                                           int *length));
char        *cMalloc            _Declare ((int size));
int         CheckDirs           _Declare ((void));
int         CheckFile           _Declare ((char *));
int         CheckOwner          _Declare ((char *, char *));
int         CheckRemoteIP       _Declare ((char *, char *));
void        Commaize            _Declare ((char *));
void        displayCountdown    _Declare ((DigitInfo *digit_info,
                                          FrameInfo *frame_info));
void        displayTime         _Declare ((DigitInfo *di,FrameInfo *fi));
void        displayDate         _Declare ((DigitInfo *di,FrameInfo *fi));
int         doHTTPcache         _Declare ((void));
void        DisplayCounter      _Declare ((void));
char        *getcfgSection      _Declare ((void));
Sllist      *getcfgIgips        _Declare ((void));
Sllist      *getcfgAhs          _Declare ((void));
char        *getdateGMT         _Declare ((void));
char        *getcfgVersion      _Declare ((void));
int     getcfgAutoFileCreation  _Declare ((void));
int     getcfgStrictMode        _Declare ((void));
int     getcfgArgbDatabase      _Declare ((void));
int     getcfgCountReload       _Declare ((void));
int     getcfgLogErrmsg         _Declare ((void));
int     getcfgShowErrmsg        _Declare ((void));
int         getExpireOffset     _Declare ((void));
void        getHTTPexpires      _Declare ((int offset,char *expires,
                                           char *date,int length,
                                           int exp_size,int date_size));
char        *getLocalTime       _Declare ((DigitInfo *di,int add_from));
int         getUserReload       _Declare ((void));
void        GetRemoteReferer    _Declare ((char *, char *));
int         GetLine             _Declare ((FILE *, char *));
char        *GetTime            _Declare ((void));
int         hostAuthorized      _Declare ((char  *host));
int         ignoreIP            _Declare ((char *remote_ip));
void        initImageParams     _Declare ((DigitInfo *,FrameInfo *));
int         isinname            _Declare ((char *string,char *mask));
void        makeGIFPath         _Declare ((char *,int,char *,DigitInfo *));
char        *mystrdup           _Declare ((char *));
char        *mystristr          _Declare ((char *s,char *t));
char        *mystrncat          _Declare ((char *dst,char *src,int n));
char        *mystrncpy          _Declare ((char *dst,char *src,int n));
char        *mystrtok           _Declare ((char *,char *));
int         mystrcasecmp        _Declare ((char *,char *));
int         mystrncasecmp       _Declare ((char *s1, char *s2, int n));
void        MogrifyImage        _Declare ((Image **,DigitInfo *,FrameInfo *));
time_t      netMktime           _Declare ((struct tm *));
char        *rwCounterDataFile  _Declare ((char *filepath,
                                          char *remote_ip,int ignore_site,
                                          int use_st,int start_value,
                                          int do_increment));
void        printList           _Declare ((Sllist *head));
void        ParseConfig         _Declare ((void));
int         ParseQueryString    _Declare ((char *, DigitInfo *, FrameInfo *));
int         ParseInteger        _Declare ((char *,int *,int,int));
int         ParseDigitString    _Declare ((char *,char *,int));
int         ParseOptionString   _Declare ((char *,int *));
int         ParseLiteralString  _Declare ((char *,char *,int));
int         ParseDateFormat     _Declare ((char *,int *));
int         ParseBool           _Declare ((char *,int *));
int         ParseRGB            _Declare ((char *,unsigned char *,
                                          unsigned char *,unsigned char *));
int         ParseSize           _Declare ((char *,int *, int *));
int         ParseError          _Declare ((char *,char *,int,char *));
void        PrintHeader         _Declare ((void));
void        releaseList         _Declare ((Sllist **head));
char        *reverseString      _Declare ((char *));
void        RemoveTrailingSp    _Declare ((char *));
void        safeStrcpy          _Declare ((char *to,char *from,int length));
void        safeStrcat          _Declare ((char *to_str,char *str,int length,
                                           int ssc_size,int ssc_length));
void        SendErrorImage      _Declare ((unsigned char *, int));
void        setCache            _Declare ((int cache));
void        setcfgSection       _Declare ((char **section));
void        setExpire           _Declare ((int exp));
void        setUserReload       _Declare ((int i));
void        SetLock             _Declare ((int));
void        StringImage         _Declare ((char *));
void        UnsetLock           _Declare ((int));
void        Warning             _Declare ((char *));
void        makeCounterImage    _Declare ((DigitInfo *, FrameInfo *));

void        multilineStringImage _Declare ((char *,int,int,int,int,int,int,int));
int         wildMat              _Declare((char *text,char *p));


#endif /* _COUNT_H */
