#include "combine.h"
#include "count.h"
#include "cdebug.h"

#ifdef SYS_WIN32
#include "configNT.h"
#elif defined( __VMS )
#include "configVMS.h"
#else
#include "config.h"
#endif

#ifdef __VMS
#define DIR_SEPARATOR "" /* Already have ] at end of _DIR macros */
#else
#define DIR_SEPARATOR "/"
#endif

/* private protos */
static int  ParseCountdown   _Declare ((char *given,int *t_year,
                                        int *t_month,int *t_day,int *t_hr,
                                        int *t_min,int *t_sec));
static int ParseTwoint       _Declare  ((char *given,
                                        int *x,
                                        int *y));




/*
 *  GetLine () - reads a line from the passed file pointer and puts the
 *               line in the passed array
 *
 *  RCS:
 *      $Revision: 1.2 $
 *      $Date: 2001/04/08 16:15:51 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      borrowed from wusage 2.3
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

int GetLine (fp, string)
FILE
    *fp;
char
    *string;
{
    int
        s,
        i;

    int
        length;

    char
        *x;
    while (!feof (fp))
    {
        x = fgets (string, 80, fp);
        if (x == (char *) NULL)
            return (0);

        if (*string == '#') /* a comment */
            continue;

        if (string[(int) strlen(string)-1] == '\n')
            string[(int) strlen(string)-1] = '\0'; /* NULL terminate*/

        if (*string == '\0')
            continue;

        length = (int) strlen(string);
        s=0;

        for (i=0; i < length; i++)
        {
            if (isspace (string[i]))
                s++;
            else
                break;
        }

        if (s)
        {
            char buf[81];
            /* copying is safe here */
            (void) strcpy (buf, string+s);
            (void) strcpy(string,buf);
        }

        length = (int) strlen(string);
        for (i=(length-1); i >= 0; i--)
        {
            if (isspace(string[i]))
                string[i]='\0';
            else
                break;
        }
        return (1);
    }
    return (0);
}

/*
 *  ParseQueryString() - parses the QUERY_STRING for Count.cgi
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
 *      char        *qs
 *      DigitInfo   *digit_info
 *      FrameInfo   *frame_info
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      DigitInfo   *digit_info
 *      FrameInfo   *frame_info
 *
 *  Return Values:
 *      value   description
 *      0   on success
 *      1   on failure
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      text
 *
 *  Development History:
 *      who                 when        why
 *      muquit@semcor.com   22-Aug-95   first cut
 */

int ParseInteger(given,sets,min,max)
char
    *given;
int
    *sets;
int
    min,
    max;
{
    if (sscanf(given, "%d", sets) == 1)
    {
        *sets = AbsoluteValue(*sets);
        if (*sets >= min && ( max == 0 || *sets <= max ))
            return(True);
    }
    return(False);
}

int ParseDigitsString(given,sets,maxlen)
char 
    *given;
char 
    *sets;
int 
    maxlen;
{
    for ( ; *given != (char) 0; given++, sets++, maxlen-- )
    {
        if (isdigit(*given) && maxlen >= 0)
            *sets = *given;
        else
            return(False);
    }
    return(True);
}

int ParseOptionString(given,sets)
char
    *given;
int
    *sets;
{
   if (mystrncasecmp(given,"counter",6) == 0)
   {
        *sets=SHOW_COUNTER;
        return (True);
   } 
   else if (mystrncasecmp(given,"clock",2) == 0)
   {
        *sets=SHOW_CLOCK;
        return (True);
   }
   else if (mystrncasecmp(given,"date",2) == 0)
   {
        *sets=SHOW_DATE;
        return (True);
   }
   else if (mystrncasecmp(given,"image",1) == 0)
   {
        *sets=SHOW_GIF_FILE;
        return (True);
   }
   else if (mystrncasecmp(given,"countdown",6) == 0)
   {
       *sets=SHOW_COUNTDOWN;
       return(True);
   }
   else if (mystrncasecmp(given,"version",3) == 0)
   {
       *sets=SHOW_VERSION;
       return(True);
   }
   else
        return (False);

return (False);
}

int ParseDateFormat(given,sets)
char
    *given;
int
    *sets;
{
    if (mystrncasecmp(given,"MMDDYY",6) == 0)
    {
       *sets=DATE_MMDDYY;
       return(True); 
    }
    else if (mystrncasecmp(given,"DDMMYY",6) == 0)
    {
       *sets=DATE_DDMMYY;
       return(True); 
    }
    else if ((mystrncasecmp(given, "YYMMDD",6) == 0) ||
             (mystrncasecmp(given, "YYYYMMDD", 8) == 0))
    {
       *sets=DATE_YYMMDD;
       return(True); 
    }
    else if ((mystrncasecmp(given,"YYDDMM",6) == 0) ||
             (mystrncasecmp(given,"YYYYDDMM", 8) == 0))
    {
       *sets=DATE_YYDDMM;
       return(True); 
    }
    else if ((mystrncasecmp(given,"MMYYDD",6) == 0) ||
             (mystrncasecmp(given,"MMYYYYDD", 8) == 0))
    {
       *sets=DATE_MMYYDD;
       return(True); 
    }
    else if ((mystrncasecmp(given,"DDYYMM",6) == 0) ||
             (mystrncasecmp(given, "DDYYYYMM", 8) == 0))
    {
       *sets=DATE_MMYYDD;
       return(True); 
    }
    else
        return (False);

return (False);
}

int ParseBool(given,sets )
char 
    *given;
int 
    *sets;
{
    if ((int)strlen(given) != 1)
        return(False);

    switch(*given)
    {
        case '1':
        case 'T':
        case 't':
        case 'Y':
        case 'y':
            *sets = True;
            break;
        case '0':
        case 'F':
        case 'f':
        case 'N':
        case 'n':
            *sets = False;
            break;
        default:
            return(False);
    }
    return(True);
}

#if __STDC__
int ParseRGB(char *given,unsigned char *set_r,unsigned char *set_g,
    unsigned char *set_b)
#else
int ParseRGB(given,set_r,set_g,set_b)
char 
    *given;
unsigned char 
    *set_r, 
    *set_g, 
    *set_b;
#endif
{
    int
        rc,
        red, 
        green, 
        blue, 
        items,
        found = False;

    FILE 
        *handle = NULL;

    char
        rgbname[MaxLineLength],
        fromfile[MaxLineLength],
        *emsg = fromfile;                   /* Reuse space! */

    rc=True;
    /*rc=sscanf(given, "%d;%d;%d",&red,&green,&blue);*/

    if ((sscanf(given, "%d;%d;%d",&red,&green,&blue) == 3) ||
        (sscanf(given,"%02x%02x%02x",&red,&green,&blue) == 3) ||
        (sscanf(given,"#%02x%02x%02x",&red,&green,&blue) == 3) ||
        (sscanf(given,"%*02x%02x%*02x%02x%*02x%02x",&red,&green,&blue) == 6)||
        (sscanf(given,"#%*02x%02x%*02x%02x%*02x%02x",&red,&green,&blue) == 6))
    {
        rc=True;
    }
    else
        rc=False;


    if (rc == False) 
    {
        handle=fopen(RGB_MAPPING_DICT, "r");
        if (handle != (FILE *) NULL)
        {
            while(fgets(fromfile, sizeof(fromfile), handle) )
            {
                /*items=sscanf(fromfile,"%d %d %d %s %*s",
                    &red,&green,&blue,rgbname);*/
                items=sscanf(fromfile,"%d %d %d %[^\n]\n",
                    &red,&green,&blue,rgbname);
                 if (items != 4)
                    continue;
                 if (mystrcasecmp(rgbname, given) == 0)
                 {
                    if ( Gdebug == True )
                       fprintf(stderr, "Found \"%d %d %d %s\" in %s\n",
                               red, green, blue, rgbname, RGB_MAPPING_DICT);
                        if (GrgbMappingIsError == 0)
                        {
                            (void) sprintf(emsg,"Please specify \"%02x%02x%02x\" instead of \"%.100s\" for color.",red,green,blue,rgbname);
                            StringImage(emsg);
                        }
                        else
                        {
                            found=True;
                            break;
                        }
                   }
            }
            (void) fclose(handle);
            if ( found == False )
                return(False);
        }
    }

    if ( red   < 0 ) red   = 0; else if ( red   > MaxRGB ) return(False);
    if ( green < 0 ) green = 0; else if ( green > MaxRGB ) return(False);
    if ( blue  < 0 ) blue  = 0; else if ( blue  > MaxRGB ) return(False);

    *set_r = (unsigned char) red;
    *set_g = (unsigned char) green;
    *set_b = (unsigned char) blue;
    return(True);
}


/*
**  ParseCountdown()
**  parses a delemited string and breaks it up
**
**  Parameters:
**  char    *given      string to parse. the items in the strings are
**                      delemeted by ; or -
**  int     *t_year     target year  - returns;
**  int     *t_month    target month  - returns;
**  int     *t_day      target day  - returns;
**  int     *t_hr       target hour - returns;
**  int     *t_min      target min - returns;
**  int     *t_sec      target sec - returns;
**
**  Return Values:
**  True if everything is ok, False otherwise
**
**  Limitations and Comments:
**  The year can be a past or future year. if past, all values will be
**  negative
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Dec-13-1998    first cut
**                           Jul-17-1999    added sanity check
*/


static int ParseCountdown(given,t_year,t_month,t_day,t_hr,t_min,t_sec)
char
    *given; 
int
    *t_year,
    *t_month,
    *t_day,
    *t_hr,
    *t_min,
    *t_sec;
{


    if ((sscanf(given,"%d;%d;%d;%d;%d;%d",
                    t_year,
                    t_month,
                    t_day,
                    t_hr,
                    t_min,
                    t_sec) == 6) ||
         (sscanf(given,"%d-%d-%d-%d-%d-%d",
                    t_year,
                    t_month,
                    t_day,
                    t_hr,
                    t_min,
                    t_sec) == 6))

    {
        /* added sanity check on Jul-17-1999 */
        if (*t_year < 0)
            return(False);

        if (*t_month < 1 || *t_month > 12)
            return(False);

        if (*t_day < 0 || *t_day > 31)
            return(False);

        if (*t_hr < 0 || *t_hr > 24)
            return(False);

        if (*t_min < 0 || *t_min > 59)
            return(False);

        if (*t_sec < 0 || *t_sec > 59)
            return(False);

        /* some jerk might provide a very large year to overflow */
        if (*t_year > MAX_COUNTDOWN_YEAR)
            return (False);
    }

     return (True);
}


/*
** parse a buffer which conatints  integer1;integer2
*/
static int ParseTwoint(given,set_x,set_y )
char
    *given;
int 
    *set_x, 
    *set_y;
{
    int
        rc;

    rc=sscanf(given, "%d;%d", set_x, set_y);

    if (rc != 2)
        return(False);

    *set_x = AbsoluteValue(*set_x);
    *set_y = AbsoluteValue(*set_y);
    return(True);
}

int ParseError(keyword, value, current_rc, blurb)
char
    *keyword,
    *value;
int
    current_rc;
char
    *blurb;
{
    char 
        emsg[MaxTextLength];


    if (strlen(keyword) + strlen(blurb) + strlen(value) + 36 > sizeof(emsg))
    {
        strcpy(emsg, "A parse error occurred with excessively long strings");
    }
    else
    {
        sprintf(emsg, "Parameter \"%s=\" followed by %s value \"%s\"", 
               keyword, blurb, value);
    }

    StringImage(emsg);
    current_rc++;
    return(current_rc);
}

/*
** returns True if OK, False otherwise
*/
static int parseAlignString(given, sets)
char
    *given;
int
    *sets;
{
    if (mystrncasecmp(given, "left", 4) == 0)
    {
        *sets = ALIGN_LEFT;
        return (True);
    }
    else if (mystrncasecmp(given, "right", 5) == 0)
    {
        *sets = ALIGN_RIGHT;
        return (True);
    }
    else if (mystrncasecmp(given, "center", 6) == 0)
    {
        *sets = ALIGN_CENTER;
        return (True);
    }
    else if (mystrncasecmp(given,"topleft",7) == 0)
    {
        *sets=ALIGN_TOP_LEFT;
        return (True);
    }
    else if (mystrncasecmp(given,"topcenter",9) == 0)
    {
        *sets=ALIGN_TOP_CENTER;
        return (True);
    }
    else if (mystrncasecmp(given,"topright",8) == 0)
    {
        *sets=ALIGN_TOP_RIGHT;
        return (True);
    }
    else if (mystrncasecmp(given,"midleft",7) == 0)
    {
        *sets=ALIGN_MID_LEFT;
        return (True);
    }
    else if (mystrncasecmp(given,"midcenter",9) == 0)
    {
        *sets=ALIGN_MID_CENTER;
        return (True);
    }
    else if (mystrncasecmp(given,"midright",8) == 0)
    {
        *sets=ALIGN_MID_RIGHT;
        return (True);
    }
    else if (mystrncasecmp(given,"bottomleft",10) == 0)
    {
        *sets=ALIGN_BOTTOM_LEFT;
        return (True);
    }
    else if (mystrncasecmp(given,"bottomcenter",12) == 0)
    {
        *sets=ALIGN_BOTTOM_CENTER;
        return (True);
    }
    else if (mystrncasecmp(given,"bottomright",11) == 0)
    {
        *sets=ALIGN_BOTTOM_RIGHT;
        return (True);
    }
    else
        return (False);
}



int ParseQueryString(qs, digit_info, frame_info)
char
    *qs;
DigitInfo
    *digit_info;
FrameInfo
    *frame_info;
{
    char
        emsg[MaxTextLength],
        query_string[MaxTextLength];

    char
        *keyword,
        *value,
        *p;

    int
        dummy;

    int
        rc=0;


    /*
    ** assumption, initImageParams() has called
    */
    safeStrcpy(query_string,qs,sizeof(query_string)-1);
    p=query_string;

    Debug2("Parsing \"%s\"",p,0);

    while((keyword=mystrtok(p, "=")) != (char *) NULL)
    {
        p = (char *) NULL;
        value=mystrtok(p,PARAM_DELIMITERS);
        if (value == (char *) NULL)
        {
            (void) sprintf(emsg,"Parameter \"%.100s=\" requires a value\n", keyword);
            StringImage(emsg);
            rc++;
            break;
        }

        Debug2("Keyword={%s}; value={%s}",keyword,value);

        /********************************************************************
        * Use of "mystrcasecmp()" is slow but fast to implement.  Will fix.
        * Error return does not occur until after entire query string is
        * parsed; this allows all errors to be shown to user in first pass.
        * Hence "rc" is incremented with each error found.
        ********************************************************************/
        if (mystrncasecmp(keyword,"ft",2) == 0)
        {
            if (!ParseInteger(value,&dummy,0,255))
                rc = ParseError(keyword,value,rc,
                    "badly specified frame thickness");
            else
                frame_info->width= (unsigned int) dummy;
        }
        else if (mystrncasecmp(keyword,"cache",3) == 0)
        {
            /* it's meaningless. expires alone is just fine */
            /* it's here for backward compatibility */
            /* muquit@muquit.com Sun Mar 25 20:14:32 EST 2001 */
            /*
            if (!ParseBool(value,&dummy))
                rc=ParseError(keyword,value,rc,"non-Boolean");
            else
                digit_info->cache=dummy;
            */
            ;
        }
 
        else if (mystrncasecmp(keyword,"expires",3) == 0)
        {
            if (!ParseInteger(value,&dummy,0,604800))
                rc=ParseError(keyword,value,rc,
                    "bad expiration value");
            else
                digit_info->expires=dummy;
        }
        else if (mystrncasecmp(keyword,"frgb",4) == 0)
        {
            if ( ! ParseRGB(value,
                &(frame_info->matte_color.red),
                &(frame_info->matte_color.green),
                &(frame_info->matte_color.blue)))
                rc = ParseError(keyword,value,rc,"badly specified RGB");

        }
        else if (mystrncasecmp(keyword,"chcolor",7) == 0)
        {
            if (!ParseBool(value, &dummy))
                rc = ParseError(keyword,value,rc,"non-Boolean");
            else
                digit_info->replace_color=(unsigned int) dummy;
        }
        else if (mystrncasecmp(keyword,"srgb",4) == 0)
        {
            if (!ParseRGB(value,
                &(digit_info->opaque_red),
                &(digit_info->opaque_green),
                &(digit_info->opaque_blue)))
                rc=ParseError(keyword,value,rc,
                    "badly specified source RGB");
        }
        else if (mystrncasecmp(keyword,"prgb",4) == 0)
        {
            if (!ParseRGB(value,
                &(digit_info->pen_red),
                &(digit_info->pen_green),
                &(digit_info->pen_blue)))
                rc=ParseError(keyword,value,rc,
                    "badly specified pen RGB");
            else
                digit_info->replace_color=True;
        }
        else if (mystrncasecmp(keyword, "trgb",4) == 0)
        {
            if ( ! ParseRGB(value,
                &(digit_info->alpha_red),
                &(digit_info->alpha_green),
                &(digit_info->alpha_blue)))
                rc = ParseError(keyword,value,rc,"badly specified RGB");
            digit_info->alpha=True;
        }
        else if (mystrncasecmp(keyword,"tr",2) == 0)
        {
            if (!ParseBool(value,&dummy))
                rc = ParseError(keyword,value,rc,"non-Boolean");
            else
                digit_info->alpha=(unsigned int) dummy;
        }
        else if (mystrncasecmp(keyword,"md",2) == 0)
        {
            if (!ParseInteger(value,&(digit_info->maxdigits),5,MAX_DIGITS))
                rc = ParseError(keyword,value,rc,"bad max digits");
        }
        else if (mystrncasecmp(keyword,"pad",3) == 0)
        {
            if (!ParseBool(value, &dummy))
                rc = ParseError(keyword,value,rc,"non-Boolean");
            else
                digit_info->leftpad=(unsigned int) dummy;
        }
        else if (mystrncasecmp(keyword,"dd",2) == 0)
        {
            /* safe copy */
            safeStrcpy(digit_info->ddhead,value,
                          sizeof(digit_info->ddhead)-1);
           Debug2("sizeof(digit_info->ddhead)=%d",sizeof(digit_info->ddhead),0);
        }
        else if (mystrncasecmp(keyword,"st",2) == 0)
        {
            if (!ParseInteger(value, &dummy, 1,999999999))
                rc = ParseError(keyword,value,rc,"bad starting");
            else
                digit_info->st=(unsigned int) dummy;
        }
        else if (mystrncasecmp( keyword, "sh",2) == 0)
        {
            if (!ParseBool(value, &dummy))
                rc = ParseError(keyword,value,rc,"non-Boolean");
            else
                digit_info->show=(unsigned int) dummy;
        }
        else if (mystrncasecmp(keyword,"istrip",6) == 0)
        {
            if (!ParseBool(value, &dummy))
               rc = ParseError(keyword,value,rc,"non-Boolean");
            else
               digit_info->use_strip=(unsigned int) dummy;

        }
        else if (mystrncasecmp(keyword,"comma",5) == 0)
        {
            if (!ParseBool(value, &dummy))
               rc = ParseError(keyword,value,rc,"non-Boolean");
            else
               digit_info->comma=(unsigned int) dummy;
        }
        else if (mystrncasecmp(keyword,"nsegment",8) == 0)
        {
            if (!ParseInteger(value, &(digit_info->nsegment), 1,999999999))
                rc = ParseError(keyword,value,rc,"bad starting");
        }
        else if (mystrncasecmp(keyword,"dformat",7) == 0)
        {
            if (!ParseDateFormat(value,&(digit_info->date_format)))
                rc=ParseError(keyword,value,rc,"invalid");
            /* if yyyy is thre, make full year on, mm Jan-27-1999 */
            if (mystristr(value,"yyyy") != NULL)
                digit_info->show_full_year=True;
            
        }
        else if (mystrncasecmp(keyword,"df",2) == 0)
        {
            safeStrcpy(digit_info->datafile,value,
                           sizeof(digit_info->datafile)-1);
        }
        else if (mystrncasecmp( keyword,"incr",4) == 0)
        {
            if (!ParseBool(value, &dummy))
                rc = ParseError(keyword,value,rc,"non-Boolean");
            else
                digit_info->increment_counter=(unsigned int) dummy;
        }
        else if (mystrncasecmp( keyword,"negate",6) == 0)
        {
            if (!ParseBool(value, &dummy))
                rc = ParseError(keyword,value,rc,"non-Boolean");
            else
                digit_info->negate=(unsigned int) dummy;
        }
        else if (mystrncasecmp( keyword,"wxh",3) == 0)
        {
            /*
            ** do nothing, this keyword is here just for backward
            ** compatibility with Count 1.5. in the current version
            ** we determine digit dimensions automatically
            */
        }
        else if (mystrncasecmp( keyword,"rotate",6) == 0)
        {
            if (!ParseBool(value, &dummy))
                rc = ParseError(keyword,value,rc,"non-Boolean");
            else
                digit_info->rotate=(unsigned int) dummy;
        }
        else if ((mystrncasecmp(keyword,"degrees",7) == 0) ||
                 (mystrncasecmp(keyword,"angle",5) == 0))
        {
            if (!ParseInteger(value,&(digit_info->rotate_degrees),90,360))
                rc = ParseError(keyword,value,rc,
                    "badly specified rotation angle");
            else
                digit_info->rotate=True;
        }
        else if (mystrncasecmp(keyword,"lit",3) == 0)
        {
           if (!ParseLiteralString(value,digit_info->literal,100))
                rc = ParseError(keyword,value,rc,"un-supported or oversized");
        }
        else if (mystrncasecmp(keyword,"display",3) == 0)
        {
            if (!ParseOptionString(value,&(digit_info->display_type)))
                rc=ParseError(keyword,value,rc,"unknown");
        }
        else if (mystrncasecmp(keyword,"image",5) == 0)
        {
            /* 
            ** this keyword does make display type image anymore. it's
            ** possible that the user is trying to composite counter, clock
            ** countdown with a base image
            */
            safeStrcpy(digit_info->gif_file,value,
                    sizeof(digit_info->gif_file)-1);
        }
        else if (mystrncasecmp(keyword,"tformat",7) == 0)
        {
            if (!ParseInteger(value,&(digit_info->time_format),12,24))
                rc=ParseError(keyword,value,rc,
                    "badly specified time format, (use 12 or 24)");
        }
        else if (mystrncasecmp(keyword,"script",6) == 0) /* for cgiwrap*/
        {
            ;
        }
        else if (mystrncasecmp(keyword,"user",4) == 0) /*for cgiwrap*/
        {
            ;
        }
        else if (mystrncasecmp(keyword,"timezone",8) == 0)
        {
            safeStrcpy(digit_info->time_z,value,
                    sizeof(digit_info->time_z)-1);
        }
        else if (mystrncasecmp(keyword,"reload",6) == 0)
        {
            if (!ParseBool(value, &dummy))
                rc = ParseError(keyword,value,rc,"non-Boolean");
            else
                digit_info->user_reload=(unsigned int) dummy;
        }
        else if (mystrncasecmp(keyword,"cdf",3) == 0)
        {
            if (! ParseCountdown(value,
                        &(digit_info->f_year),
                        &(digit_info->f_mon),
                        &(digit_info->f_day),
                        &(digit_info->f_hr),
                        &(digit_info->f_min),
                        &(digit_info->f_sec)))
            {
                   rc=ParseError(keyword,value,rc,
                           "badly specified cowntdown from");
            }
            digit_info->display_type=SHOW_COUNTDOWN;
        }
        else if ((mystrncasecmp(keyword,"cdt",3) == 0) ||
                 (mystrncasecmp(keyword,"target",6) == 0))
        {
            if (! ParseCountdown(value,
                        &(digit_info->t_year),
                        &(digit_info->t_mon),
                        &(digit_info->t_day),
                        &(digit_info->t_hr),
                        &(digit_info->t_min),
                        &(digit_info->t_sec)))
            {
                   rc=ParseError(keyword,value,rc,
                           "badly specified cowntdown target");
            }
            digit_info->display_type=SHOW_COUNTDOWN;
        }
        else if (mystrncasecmp(keyword,"sfd",3) == 0)
        {
            /* show from date of countdown below the image */
            if (!ParseBool(value, &dummy))
               rc = ParseError(keyword,value,rc,"non-Boolean");
            else
               digit_info->show_from_date=(int) dummy;
        }
        else if (mystrncasecmp(keyword,"sdhms",5) == 0)
        {
            if (!ParseBool(value,&dummy))
                rc=ParseError(keyword,value,rc,"non-Boolean");
            else
                digit_info->show_dhms=(int) dummy;
        }
        else if (mystrncasecmp(keyword,"offset",6) == 0)
        {
            if (!ParseInteger(value,&dummy,0,999999999))
                rc = ParseError(keyword,value,rc,
                    "badly specified frame offset");
            else
                digit_info->offset= (unsigned int) dummy;
        }
        else if (mystrncasecmp(keyword,"xy",2) == 0)
        {
            if (! ParseTwoint(value,
                        &(digit_info->upl_x),
                        &digit_info->upl_y))
            {
                   rc=ParseError(keyword,value,rc,"badly specified sixy");
            }
        }
        else if (mystrncasecmp(keyword,"align",5) == 0)
        {
            if (!parseAlignString(value,&(digit_info->alignment)))
                rc=ParseError(keyword,value,rc,"unknown");
            digit_info->align=True;
        }
        else if (mystrncasecmp(keyword,"fy",2) == 0)
        {
            /* show full year instead of 2 digit year */
            if (!ParseBool(value, &dummy))
                rc = ParseError(keyword,value,rc,"non-Boolean");
            else
                digit_info->show_full_year=(unsigned int) dummy;
        }
        else 
        {
            sprintf(emsg, "Bad keyword \"%.100s\"", keyword);
            StringImage(emsg);
            rc++;
        }
    }

    return(rc);
}

/*
 *  GetRemoteReferer - returns the remote referer
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
 *      char    *host       remote referer HTML
 *
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      char    *rem_host   retuns the host
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
 *      muquit@semcor.com   12-Aug-95   first cut
 */

void GetRemoteReferer (host, rem_host)
char
    *host;
char
    *rem_host;
{
    register char
        *p,
        *q;

    int
        x;

    *rem_host = '\0';
    q=rem_host;

    for (p=host; *p != '\0'; p++)
    {
        if (*p == '/')
        {
            p++;
            if (*p == '/')
            {
                p++;
            }
            break;
        }
    }
    while ((*p != '/') && (*p != '\0'))
        *q++ = *p++;

    *q = '\0';

    /*
    ** randerso@bite.db.uth.tmc.edu added the following lines of code
    ** to account for port numbers at the end of a url
    */

    x=0;
    while ((x < (int) strlen(rem_host)) && (rem_host[x] != ':'))
        x++;
    rem_host[x]='\0';
}

#ifdef SYS_UNIX
void SetLock (fd)
int
    fd;
{
#ifdef HAVE_FLOCK
    (void) flock(fd,LOCK_EX);
#else
    lseek(fd,0L,0);
    (void) lockf(fd,F_LOCK,0L);
#endif
}

void UnsetLock (fd)
int
    fd;
{
#ifdef HAVE_FLOCK
    (void) flock(fd,LOCK_UN);
#else
    lseek(fd,0L,0);
    (void) lockf(fd,F_ULOCK,0L);
#endif
}
#endif  /* SYS_UNIX */

#ifndef HAVE_MKTIME
/*
 * $Author: muquit $
 * $Date: 2001/04/08 16:15:51 $
 * $Id: parse.c,v 1.2 2001/04/08 16:15:51 muquit Exp $
 * $Source: /usr/local/cvs/wwwcount2.5/src/parse.c,v $
 *
 * Grr.. on EP/IX I have no mktime(2) routine so I've had to roll my own.
 * Original work 7-Apr-95 by Reg Quinton <reggers@julian.uwo.ca>
*/


/* this is awfully nasty, the epoch began in 1970 UTC and 1968 was
   the preceding leap year. This converts a time structure to the
   number of seconds since the leap year. But assumes I have all
   sorts of good values which might not be there and doesn't do
   any fancy work wrt. time zones (assumes date given is same
   timezone and EST/EDT offset) which obviously isn't true. */

time_t netMktime(v)
struct tm *v;
{
struct	tm	*tm;
	int	leap;
	time_t	n;

	leap= (((v)->tm_year) % 4) ? 0 : 1;

	switch ((v)->tm_mon) {
	case 0:	/* Jan */
		(v)->tm_yday=(v)->tm_mday - 1;
		break;;
	case 1: /* Feb */
		(v)->tm_yday=31 + (v)->tm_mday - 1;
		break;;
	case 2: /* Mar */
		(v)->tm_yday=59 + (v)->tm_mday - 1 + leap;
		break;;
	case 3: /* Apr */
		(v)->tm_yday=90 + (v)->tm_mday - 1 + leap;
		break;;
	case 4: /* May */
		(v)->tm_yday=120 + (v)->tm_mday - 1 + leap;
		break;;
	case 5: /* Jun */
		(v)->tm_yday=151 + (v)->tm_mday - 1 + leap;
		break;;
	case 6: /* Jul */
		(v)->tm_yday=181 + (v)->tm_mday - 1 + leap;
		break;;
	case 7: /* Aug */
		(v)->tm_yday=212 + (v)->tm_mday - 1 + leap;
		break;;
	case 8: /* Sep */
		(v)->tm_yday=243 + (v)->tm_mday - 1 + leap;
		break;;
	case 9: /* Oct */
		(v)->tm_yday=273 + (v)->tm_mday - 1 + leap;
		break;;
	case 10: /* Nov */
		(v)->tm_yday=304 + (v)->tm_mday - 1 + leap;
		break;;
	case 11: /* Dec */
		(v)->tm_yday=334 + (v)->tm_mday - 1 + leap;
		break;;
	}

	/* n is the time value GMT, I need an offset */

	n=(((((((((((v)->tm_year) - 70) * 365 + 
                          (((v)->tm_year) - 68) / 4)  + 
                           ((v)->tm_yday)) * 24 ) +
                           ((v)->tm_hour)) * 60 ) + 
                           ((v)->tm_min))  * 60 ) + 
                           ((v)->tm_sec) );

	/* find the GMT offset and take it off, this fails
	   real bad at the time the timezone changes */

	tm=localtime(&n);	return(n-tm->tm_gmtoff);
}

#endif /* !HAVE_MKTIME */

/*
** parse literal string
** 04/23/96
*/
int ParseLiteralString(given,sets,maxlen)
char
    *given;
char
    *sets;
int
    maxlen;
{
    for (; *given != (char) 0; given++, sets++,maxlen--)
    {
        if (((isdigit(*given) ||
             (*given == ':')  ||
             (*given == ',')  ||
             (*given == '-')  ||
             (*given == 'D')  ||
             (*given == 'H')  ||
             (*given == 'M')  ||
             (*given == 'S')  ||
             (*given == 'a')  ||
             (*given == 'A')  ||
             (*given == 'p')  ||
             (*given == 'P')) &&
            (maxlen >= 0)))
        {
            *sets=*given;
        }
        else
            return (False);
    }
    return (True);
}


/*
**  initImageParams()
**  initialized digit_info and frame_info
**
**  RCS
**      $Revision: 1.2 $
**      $Date: 2001/04/08 16:15:51 $
**  Return Values:
**      none
**
**  Parameters:
**      digit_info      returns
**      frame_info      returns
**
**  Side Effects:
**      digit_info and frame_info are changed
**
**  Limitations and Comments:
**      none
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Nov-30-1997    first cut
*/

void initImageParams(digit_info,frame_info)
DigitInfo
    *digit_info;
FrameInfo
    *frame_info;
{
    frame_info->width=(unsigned int) DEFAULT_FRAME_THICKNESS;
    frame_info->matte_color.red=(unsigned char) DEFAULT_FRAME_RED;
    frame_info->matte_color.green=(unsigned char) DEFAULT_FRAME_GREEN;
    frame_info->matte_color.blue=(unsigned char) DEFAULT_FRAME_BLUE;

    digit_info->pdigits=(char *) NULL;

    digit_info->replace_color=False;
    digit_info->cache=1;        /* not used MM Sun Mar 25 20:14:54 EST 2001  */
    digit_info->expires=-1;    
    digit_info->opaque_red=(unsigned char) DEFAULT_OPAQUE_RED;
    digit_info->opaque_green=(unsigned char) DEFAULT_OPAQUE_GREEN;
    digit_info->opaque_blue=(unsigned char) DEFAULT_OPAQUE_BLUE;

    digit_info->pen_red=(unsigned char) DEFAULT_PEN_RED;
    digit_info->pen_green=(unsigned char) DEFAULT_PEN_GREEN;
    digit_info->pen_blue=(unsigned char) DEFAULT_PEN_BLUE;

    digit_info->alpha=(unsigned int) DEFAULT_TRANSPARENCY;
    digit_info->width=(unsigned int) 15;    /* never used */
    digit_info->height=(unsigned int) 20;   /* never used */
    digit_info->maxdigits=(int) DEFAULT_MAXDIGITS;
    digit_info->leftpad=(unsigned int) DEFAULT_LEFTPAD;
    digit_info->alpha_red=(unsigned char) DEFAULT_TRANSPARENT_RED;
    digit_info->alpha_green=(unsigned char) DEFAULT_TRANSPARENT_GREEN;
    digit_info->alpha_blue=(unsigned char) DEFAULT_TRANSPARENT_BLUE;
    digit_info->st=(unsigned int) DEFAULT_STARTCOUNT;
    digit_info->show=(unsigned int) DEFAULT_SHOWCOUNT;
    digit_info->increment_counter=(unsigned int) True;
    digit_info->negate=False;
    digit_info->rotate=False;
    digit_info->rotate_degrees=270;
    safeStrcpy(digit_info->datafile,DEFAULT_DATAFILE,
            sizeof(digit_info->datafile)-1);
    Debug2("sizeof(digit_info->datafile)=%d",sizeof(digit_info->datafile),0);
    safeStrcpy(digit_info->ddhead,DEFAULT_DIGITS_STYLE,
            sizeof(digit_info->ddhead)-1);

    Debug2("sizeof(digit_info->ddhead)=%d",sizeof(digit_info->ddhead),0);
    *digit_info->literal = '\0';
    digit_info->display_type=SHOW_COUNTER;
    *digit_info->time_z='\0';
    digit_info->date_format=DATE_MMDDYY;
    digit_info->time_format=12;
    digit_info->use_strip=True;
    digit_info->comma=False;
    digit_info->nsegment=15;
    *digit_info->gif_file='\0';
    digit_info->user_reload=1;

    /* dec-13-1998, mm */
    /* default countdown targe is Year 2000 */
    digit_info->t_year=2000;
    digit_info->t_mon=1;
    digit_info->t_day=1;
    digit_info->t_hr=0;
    digit_info->t_min=0;
    digit_info->t_sec=0;

    /* 
    ** default countdown from values, all negative, so the user must specify
    ** them
    */
    digit_info->f_year=(-1);
    digit_info->f_mon=(-1);
    digit_info->f_day=(-1);
    digit_info->f_hr=(-1);
    digit_info->f_min=(-1);
    digit_info->f_sec=(-1);



    /* dont' show from below the countdown time by default */
    digit_info->show_from_date=0;

    /* show day hr min sec in countdown by default */
    digit_info->show_dhms=1;

    digit_info->upl_x=(-1);
    digit_info->upl_y=(-1);

    digit_info->show_full_year=False;

    digit_info->align=False;
    digit_info->alignment=ALIGN_TOP_CENTER;
    digit_info->offset=2;

}

