/*
 *  WWW document access counter
 *
 *  RCS:
 *      $Revision: 1.2 $
 *      $Date: 2001/04/08 16:15:51 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *     main routine for WWW homepage access counter
 *  
 *  Input Parameters:
 *      type    identifier  description
 *
 *      N/A
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      N/A
 *
 *  Return Values:
 *      value   description
 *
 *  Side Effects:
 *      None
 *
 *  Limitations and Comments:
 *      None
 *
 *  Development History:
 *      who                 when        why
 *      muquit@semcor.com   10-Apr-95   first cut
 *      muquit@semcor.com   06-07-95    release 1.2
 *      muquit@semcor.com   07-13-95    release 1.3
 *      ma_muquit@fccc.edu  10-11-97    release 2.3
 */

#define __Main__

#ifdef __VMS
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "cdebug.h"
#include "combine.h"
#include "count.h"

#ifdef __VMS
#include <fcntl.h>
#include <unixio.h>
#endif

#ifdef SYS_WIN32
#include "configNT.h"
#elif defined( __VMS )
#include "configVMS.h"
#else
#include "config.h"
#endif

#ifdef __VMS
# define DIR_SEPARATOR "" /* Already have ] at end of _DIR macros */
/* VMS DIGIT_DIR ends in . so need to add subdirectory and ] - handled later */
#define SYM_PREFIX "WWW_"
#else
#define DIR_SEPARATOR "/"
#define SYM_PREFIX ""
#endif

int main(argc,argv)
int
    argc;
char
    **argv;
{
    int
        i,
        rc=0,
        count,
        counter,
        counter_length,
        left_pad,
        ihas_referer=0,
        max_digits=MAX_DIGITS,
        display_what;

    unsigned int
        use_st=False,
        ignore_site=False;

#ifdef SYS_WIN32
	char
		*script_name=NULL,
	    drive[16];

#endif /* SYS_WIN32 */

    char
        hour[10],
        min[10],
        ampm[10],
        *s,
        *dbuf=(char *) NULL,
        *browser_type,
        *penv,
        *remote_ip,
        *query_string,
        filename[MaxTextLength],
        tmpbuf[MaxTextLength],
        digitbuf[MaxTextLength];

    DigitInfo
        digit_info;

    FrameInfo
        frame_info;

/*
**---------initialize globals------Starts---
*/
    Gdebug=False;
    GrgbMappingIsError=1;

#ifdef SYS_WIN32
    *g_sZ_vbase='\0';
#endif
    *g_sZ_ibase='\0';



/*
**---------initialize globals------Ends---
*/
    count=1;
    *hour='\0';
    *min='\0';
    *ampm='\0';
    *filename = '\0';
    *digitbuf='\0';
    *tmpbuf = '\0';
    counter = 0;
    left_pad=max_digits;
    use_st=False;
    display_what=SHOW_COUNTER;
    penv=(char *) NULL;
    remote_ip=(char *) NULL;


    /*
    ** parse command line, this is only used for testing at commandline
    ** no command line flag is allowed in the web page while calling
    ** the program
    */

    for (i=1; i < argc; i++)
    {
        if (!strncmp(argv[i],"-debug",(int)strlen("-debug")))
        {
            Gdebug=True;
            break;
        }
        s="-version";
        if(!strncmp(argv[i],s,(int) strlen(s)))
        {
            (void) fprintf(stderr,"Count version %s\n",IMAGE_COMMENT);
            exit(1);
        }
    }


#ifdef SYS_WIN32
    /*
    ** store the path of cgi program. we need this for relative
    ** access of the counter resources.
    */
    script_name=argv[0];

    *tmpbuf='\0';

    /*
    ** I've no idea how _splitpath()  handles buffer overflow
    */
    _splitpath(script_name,drive,tmpbuf,NULL,NULL);

    safeStrcpy(g_sZ_vbase,drive,sizeof(g_sZ_vbase)-1);
    safeStrcat(g_sZ_vbase,tmpbuf,sizeof(g_sZ_vbase)-1,
            sizeof(g_sZ_vbase),strlen(g_sZ_vbase));
    /*
    ** note, g_sZ_vbase will have a trailing \
    ** example: cgi-bin\  in EMWAC
    ** c:\usr\local\etc\WebSite\cgi-shl\    for WebSite 1.1e
    ** c:\InetPub\scripts\ for MS IIS server
    */

    safeStrcpy(g_sZ_ibase,g_sZ_vbase,sizeof(g_sZ_ibase)-1);
    safeStrcat(g_sZ_ibase,DIGIT_DIR,sizeof(g_sZ_ibase)-1,
            sizeof(g_sZ_ibase),strlen(g_sZ_ibase));
 
#else
    safeStrcpy(g_sZ_ibase,DIGIT_DIR,sizeof(g_sZ_ibase)-1);
#endif  /* SYS_WIN32 */
    

    /* get the remote IP, we'll need it in many places */
#ifdef __VMS
    penv=(char *) getenv(SYM_PREFIX "HTTP_X_FORWARDED_FOR");
    if (penv == NULL)
        penv = (char *) getenv(SYM_PREFIX "REMOTE_ADDR");
#else
    penv=(char *) getenv("HTTP_X_FORWARDED_FOR");
    if (penv == NULL)
        penv = (char *) getenv("REMOTE_ADDR");
#endif

    if (penv == (char *) NULL)
        remote_ip="0.0.0.0";
    else
        remote_ip=mystrdup(penv);
 
    Debug2("[%s] Count %s: --Debug--",GetTime(),Version);

    rc=CheckDirs();
    if (rc == 1)
    {
        *tmpbuf='\0';
        safeStrcpy(tmpbuf,"CONFIG_DIR,DIGIT_DIR,DATA_DIR,LOG_DIR must differ",
                sizeof(tmpbuf)-1);
        Warning(tmpbuf);

        StringImage(tmpbuf);
        exit(1);
    }

    /*
    ** parse the config file
    */
    ParseConfig ();



    /* print what we read, for debug only....... Remore asap */

    /*
    ** we will be only be here if everything is fine, otherwise we'll be
    ** out from ParseConfig()
    */



    /*
    ** check if the referer is a remote host or not. refere should
    ** be the local host.
    */
    if (getcfgStrictMode() == True)
    {
        ihas_referer=authRemhost();
    } 
    /* if not authorized, we won't be here */

 

   /*
    ** get the user name from query string
    */
#ifdef __VMS
    query_string = (char *) getenv(SYM_PREFIX "QUERY_STRING");
#else
    query_string = (char *) getenv("QUERY_STRING");
#endif

    if ((query_string == (char *) NULL) || (*query_string == '\0'))
    {
        *tmpbuf='\0';
        safeStrcpy(tmpbuf,"Empty QUERY_STRING",sizeof(tmpbuf)-1);
        Warning(tmpbuf);
        StringImage(tmpbuf);
        exit(1);
    }

    /*
    ** initialize to default values
    */
    initImageParams(&digit_info,&frame_info);


    rc=ParseQueryString(query_string,&digit_info,&frame_info);
    if (rc)
    {
        Debug2("Error parsing QUERY_STIRNG",0,0);

        StringImage("Error parsing QUERY_STRING");
        exit(0);
    }

    setUserReload(digit_info.user_reload);

#if 0

    Debug2("\n",0,0);
    Debug2("Parsed QUERY_STRING",0,0); 
    Debug2(" ft=%d",frame_info.width,0);
    Debug2(" cache=%d", digit_info.cache,0);
    Debug2(" expires=%d",digit_info.expires,0);
    Debug4(" rgb=%d,%d,%d", frame_info.matte_color.red,
        frame_info.matte_color.green, frame_info.matte_color.blue,0);
    Debug2(" tr=%d",digit_info.alpha,0);
    Debug4(" trgb=%d,%d,%d",digit_info.alpha_red,
        digit_info.alpha_green,digit_info.alpha_blue,0);
    Debug2(" replace_color=%d",digit_info.replace_color,0);
    Debug4(" srgb=%d,%d,%d",
        digit_info.opaque_red,
        digit_info.opaque_green,
        digit_info.opaque_blue,
        0);
    Debug4(" prgb=%d,%d,%d",
        digit_info.pen_red,
        digit_info.pen_green,
        digit_info.pen_blue,
        0);
    Debug2(" maxdigits=%d",digit_info.maxdigits,0);
    Debug2(" wxh [ignored,exists for compatibity] =%dx%d", 
        digit_info.width, digit_info.height);
    Debug2(" md=%d", digit_info.maxdigits,0);
    Debug2(" pad=%d", digit_info.leftpad,0);
    Debug2(" dd=%s",digit_info.ddhead,0);
    Debug2(" st=%d",digit_info.st,0);
    Debug2(" sh=%d", digit_info.show,0);
    Debug2(" df=%s",digit_info.datafile,0);
    Debug2(" lit=%s",digit_info.literal,0);
    Debug2(" incr=%d",digit_info.increment_counter,0);
    Debug2(" negate=%d",digit_info.negate,0);
    Debug2(" user_reload=%d",digit_info.user_reload,0);
    Debug2(" rotate=%d",digit_info.rotate,0);
    Debug2(" degrees=%d",digit_info.rotate_degrees,0);
    Debug2(" timezone=%s",digit_info.time_z,0);
    Debug2(" timeformat=%d",digit_info.time_format,0);
    Debug2(" istrip=%d",digit_info.use_strip,0);
    Debug2(" image=%s", digit_info.gif_file,0);
    Debug2(" sfd=%d",digit_info.show_from_date,0);
    Debug2(" parsed QS--\n",0,0);
#endif /* if 0 */


    /*
    ** now set some values for future retrieval
    */
    /*
    ** well, cache is not used anymore.. it's there just for backward
    ** compatibility. Only use "expires". Using cache and expires together
    ** creates too much confusion. If you to tell the browser not to cache
    ** specify expires=0, speicfy a value greater than 0 if want the browser
    ** to cache for that amount of seconds.
    ** muquit@muquit.com Sun Mar 25 20:01:46 EST 2001 
    */
/*
    setCache(digit_info.cache);
*/
    setExpire(digit_info.expires);


    /*
    ** check if digit head has any path in it
    ** a bad bug, reported by Ojanpera <ojapa@japo.fi>
    */

    if (checkfilename (digit_info.ddhead) != 0)
    {
        *tmpbuf='\0';
        safeStrcpy(tmpbuf,"Illegal digit path!",sizeof(tmpbuf)-1);
        Warning(tmpbuf);

        StringImage(tmpbuf);
        exit(0);
    }


    if (digit_info.leftpad == True)
    {
        max_digits=digit_info.maxdigits;

        /* check for minimum max_digits */
        if (max_digits < 5)
            max_digits=5;
        if (max_digits > MAX_DIGITS)
            max_digits=MAX_DIGITS;
    }

    /*
    ** now find out what we'r trying to display
    */
    display_what=digit_info.display_type;

    if (display_what == SHOW_COUNTER)
    {
        if (checkfilename (digit_info.datafile) != 0)
        {
            *tmpbuf='\0';
            safeStrcpy(tmpbuf,"Illegal datafile path!",sizeof(tmpbuf)-1);
            Warning(tmpbuf);

            StringImage(tmpbuf);
            exit(1);
        }
    }
    /*
    ** get frameinfo
    */

    if (frame_info.width > 1)
        digit_info.Frame=True;
    else
        digit_info.Frame=False;
    /*
    ** get the IP address of the connected machine to check if we need
    ** to increment the counter
    */
    if (display_what == SHOW_COUNTER)
    {
       if (remote_ip == (char *) NULL)
       {
            /*
            ** put a dummy here
            */
            remote_ip = mystrdup("dummy_ip");
        }
        else
        {
            ignore_site=ignoreIP(remote_ip);
            if (ignore_site == True)
            {
                Debug2("ignoring site: %s",remote_ip,0);
            }
        }
    }

    /*
    ** initialize Limit array
    */


    if (getcfgStrictMode() == True)
    {
        if (ihas_referer == 0)
        {
            /*
            ** no HTTP_REFERER env variable found.
            ** display a literal string. this will make sure that even if
            ** someone with a broken browser tries to access the counter
            ** remotely, will not mess up the datafile. of course we'll
            ** miss some counts. because anyone accesses the page with a 
            ** broken browser like this will see a literal string
            */
            (void) strcpy(digit_info.literal,"888888");
            strcpy(digitbuf,"888888");
            /*
            ** if gif file was asked to display,we'll be in trouble
            */
            initImageParams(&digit_info,&frame_info);
            digit_info.Frame=False;
            digit_info.pdigits=mystrdup(digitbuf);
            makeCounterImage(&digit_info,&frame_info);

            /*
            ** we won't be here
            */
            exit (0);

        /*
        if (remote_ip != (char *) NULL)
        {
            (void) sprintf(tmpbuf,"No HTTP_REFERER supplied from %s",
                remote_ip);
            Warning(tmpbuf);
        }
        */
        /*
        ** try to get the browser name, it will give us a good
        ** feedback.
        */
            if (getcfgLogErrmsg() == True)
            {

#ifdef __VMS
                browser_type=(char *) getenv(SYM_PREFIX "HTTP_USER_AGENT");
#else
                browser_type=(char *) getenv("HTTP_USER_AGENT");
#endif
                if ((browser_type != (char *) NULL) &&
                    (*browser_type != '\0'))
                {
                    if (remote_ip != (char *) NULL)
                    {
                        *tmpbuf='\0';
                        safeStrcpy(tmpbuf,browser_type,sizeof(tmpbuf)-1);
                        safeStrcpy(digitbuf,"browser without referer ",
                                sizeof(digitbuf)-1);
                        safeStrcat(digitbuf,tmpbuf,sizeof(digitbuf)-1,
                                sizeof(digitbuf),strlen(digitbuf));
                        Warning(digitbuf);
                        *digitbuf='\0';
                    }
                }
            }
        }
    }   /*getcfgStringMode()  == True */

    /*
    ** if gif file is specified, make the full path
    */
    if (*digit_info.gif_file != '\0')
    {
        
        /*
        ** security fix. oct-10-1997
        ** first check if the image file specified with image=
        ** parameter has any path in it
        */
        if (checkfilename(digit_info.gif_file) != 0)
        {
            StringImage("Illegal image filename!");
            exit(0);
        }

        makeGIFPath(tmpbuf,sizeof(tmpbuf),g_sZ_ibase,&digit_info);
        /* overwrite digit_info.gif_file */
        safeStrcpy(digit_info.gif_file,tmpbuf,1023);
    }

    switch (display_what)
    {

        case SHOW_CLOCK: /* act as a clock*/
        {
            displayTime(&digit_info,&frame_info);
            /*
            ** we will not be here
            */
            exit(0);
            break;
        }

        case SHOW_DATE:
        {
            displayDate(&digit_info,&frame_info);
            /*
            ** we won't be here
            */
            break;
        }

        case SHOW_COUNTDOWN:
        {
            displayCountdown(&digit_info,&frame_info);
            break;
        }
        case SHOW_VERSION:
        {
            multilineStringImage(IMAGE_COMMENT,ALIGN_CENTER,0,0,0,0,255,0);
            exit(0);
            break;
        }

        case SHOW_GIF_FILE:
        {
            if (*digit_info.gif_file == '\0')
            {
                StringImage("No GIF file specified to display");
                exit(1);
            }
            /* 
            ** check if counter is needed to be incremented
            */

            if (*digit_info.datafile != '\0')
            {
                Debug2("datafile=\"%s\"",digit_info.datafile,0);

                /*
                ** if datafile is RANDOM, just ignore counting
                */
                if (mystrcasecmp(digit_info.datafile,"random") == 0)
                    digit_info.increment_counter=False;
                /*
                ** check increment. if true, we'll make the path of the
                ** datafile. Note: we'll never display it, just increment
                */
                if (digit_info.increment_counter)
                {
                    Debug2("increment counter behind the image!",0,0);
                    /* check if some idiot is trying to screw up */
                    if (checkfilename(digit_info.datafile))
                    {
                        StringImage("Illegal character in filename!");
                        exit(0);
                    }
#ifdef SYS_WIN32
                    safeStrcpy(filename,g_sZ_vbase,sizeof(filename)-1);
                    safeStrcat(filename,DATA_DIR,sizeof(filename)-1,
                            sizeof(filename),strlen(filename));
                    
#else
                    safeStrcpy(filename,DATA_DIR,sizeof(filename)-1);
#endif
                    safeStrcat(filename,DIR_SEPARATOR,sizeof(filename)-1,
                            sizeof(filename),strlen(filename));
                    safeStrcat(filename,digit_info.datafile,
                            sizeof(filename)-1,sizeof(filename),
                            strlen(filename));

                    Debug2("datafile path=%s",filename,0);
                    if (remote_ip)
                        ignore_site=ignoreIP(remote_ip);
                    /* 
                    ** NOTE: dbuf is malloc'd but we won't care to free it
                    ** as memory will be freed when the CGI exits
                    */
                    dbuf=rwCounterDataFile(filename,
                            remote_ip,ignore_site,False,
                            1,digit_info.increment_counter);

                }

                /*
                ** if literal string is specified, it might be later used
                ** composite. mm, jan-18-1999
                */
                if ((int)strlen(digit_info.literal) > 0)
                {
                    dbuf=mystrdup(digit_info.literal);
                }
            }

           /*
            ** we will do some bad thing here. the gif_file member of
            ** the DigitInfo structure is not used anymore. so we will
            ** put the counter digit buffer into it. Just for my own
            ** future reference
            ** mm, dec-19-1998
            */
            if (dbuf != NULL)
                digit_info.pdigits=mystrdup(dbuf);
            else
                digit_info.pdigits=mystrdup(digitbuf);

            makeCounterImage(&digit_info,&frame_info);
            /* we won't be here */

            break;
        }
        case SHOW_COUNTER:
        default:
        {

            if (digit_info.comma == True)
                digit_info.leftpad=False;

            if ((int) strlen(digit_info.literal) > 0)
            {
                /* digitbuf buffer must be larger than MAX_DIGITS */
                   safeStrcpy(digitbuf,digit_info.literal,MAX_DIGITS-1);

                /* need to look at it again --------------------*/
                if (digit_info.comma == True)
                {
                    /* make sure all are digits, if not turn off comma */
                    for (i=0; i < strlen(digitbuf); i++)
                    {
                        if (!isdigit(digitbuf[i]))
                        {
                            Debug2("non digit in lit, turning off comma",0,0);
                            digit_info.comma=False;
                            break;
                        }
                    }
                }
                digit_info.leftpad=False;
                dbuf=mystrdup(digitbuf);
                if (dbuf == (char *) NULL)
                {
                    StringImage("memory allocation problem");
                    exit(0);
                 }
            }
            else if (mystrcasecmp(digit_info.datafile,DF_FOR_RANDOM) == 0)
            {
                srand(time(NULL));  /* Seed number generator */
                counter = rand();   /* My psychic prediction of counter value */

                /*
                ** we do not want to overflow buffer
                */
                Debug2("random number=%d",counter,0);

                *tmpbuf='\0';
                (void) sprintf(tmpbuf,"%d",counter);

                /* 
                ** I don't know why I did the following?
                ** Nov-03-1997. muquit
                */
                /*
                if (strlen(tmpbuf) > 10)
                    safeStrcpy(digitbuf,tmpbuf,10);
                else
                */

              safeStrcpy(digitbuf,tmpbuf,sizeof(digitbuf)-1);
              digit_info.leftpad=False;
              dbuf=mystrdup(digitbuf);
              if (dbuf == (char *) NULL)
              {
                  StringImage("memory allocation problem");
                  exit(0);
              }
            }
            else
            {
#ifdef SYS_WIN32
                    safeStrcpy(filename,g_sZ_vbase,sizeof(filename)-1);
                    safeStrcat(filename,DATA_DIR,sizeof(filename)-1,
                            sizeof(filename),strlen(filename));
#else
                safeStrcpy(filename,DATA_DIR,sizeof(filename)-1);
#endif
                safeStrcat(filename,DIR_SEPARATOR,sizeof(filename)-1,
                        sizeof(filename),strlen(filename));
                safeStrcat (filename,digit_info.datafile,sizeof(filename)-1,
                        sizeof(filename),strlen(filename));

                /*
                ** check if the counter file exists or not
                */
                if (getcfgAutoFileCreation() == False)
                {
                    if (CheckFile (filename) != 0)
                    {
                        if (strlen(filename) > sizeof(tmpbuf)-60)
                        {
                            StringImage("buffer overflow in filename");
                            exit(0);
                        }
                        *tmpbuf='\0';
                        (void) sprintf (tmpbuf,
                    "Counter datafile \"%s\" must be created first!", filename);
                        Warning(tmpbuf);
                        StringImage (tmpbuf);
                        exit(1);
                    }

                }
                else
                {
                    if (CheckFile (filename) != 0)
                        use_st=True;
                }

                /*
                ** read-write the counter datafile
                */

                dbuf=rwCounterDataFile(filename,
                        remote_ip,ignore_site,use_st,
                        digit_info.st,digit_info.increment_counter);
    
            }
    /* temporary things */

    Debug2("dbuf=%s",dbuf,0);

    if (dbuf != (char *) NULL)
    {
        safeStrcpy(digitbuf,dbuf,sizeof(digitbuf)-1);
        /* we don't need it anymore */
        (void) free(dbuf);
        dbuf=(char *) NULL;
    }
    else
    {
        StringImage("unknown problem!");
        exit(0);
    }

    counter_length = (int) strlen(digitbuf);

        if (digit_info.show == False)
        {
            Image
                *image;

            image=CreateBaseImage(1,1,0,0,0,DirectClass);
            if (image == (Image *) NULL)
            {
                StringImage("Failed to create 1x1 GIF image");
                exit(1);
            }

            AlphaImage(image,0,0,0);
            PrintHeader();
            (void) WriteGIFImage (image, (char *)NULL);
            DestroyAnyImageStruct (&image);
            exit(0);
        }

       if (digit_info.leftpad == False)    /* no left padding */
       {
            digit_info.pdigits=mystrdup(digitbuf);
            makeCounterImage(&digit_info,&frame_info);
            /*
            ** we will not be here, we'll exit from LoadDigits()
            */
        }
        else
        {
            if (counter_length < max_digits)
                left_pad = max_digits - counter_length;
            else
                left_pad=0;

              if ((left_pad < max_digits) && (left_pad != 0))
              {

                if (left_pad > sizeof(tmpbuf)-1)
                {
                    StringImage("buffer overflow detected in left_pad");
                    exit(0);
                }
                (void) strcpy(tmpbuf,"0");
                for (i=1; i < left_pad; i++)
                {
                    (void) sprintf(tmpbuf,"%s0",tmpbuf);
                }
                (void) sprintf(tmpbuf,"%s%s",tmpbuf,digitbuf);
                digit_info.pdigits=mystrdup(tmpbuf);
                makeCounterImage(&digit_info,&frame_info);
                /*
                ** we will not be here
                */
              }
              else    /* MaxDigits*/
              {
                  digit_info.pdigits=mystrdup(digitbuf);
                  makeCounterImage(&digit_info,&frame_info);
                /*
                ** we won't be here
                */
               }

            }
            exit(0);
        } /* case SHOW_COUNTER*/
    }

    exit(0);
}
