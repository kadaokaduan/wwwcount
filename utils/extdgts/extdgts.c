/*
 *  main - for extdgts
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2001/03/19 01:59:51 $
 *
 *  Description:
 *
 *      This program extracts the inidividual digits from the strip.gif
 *      and name the images as one.gif,two.gif.....
 *
 *  Limitations and Comments:
 *      This program reads the GIF comment extension for determining how
 *      many digits in the strip. The strip must be created with mkstrip
 *      program in order to work.
 *
 *
 *  Development History:
 *      who                  when       why
 *      ma_muquit@fccc.edu   Dec-07-96  first cut
 */

#define __Main__
#include "cdebug.h"
#include "combine.h"
#include "count.h"


#ifdef SYS_WIN32 
#include "configNT.h"
#else
#include "config.h"
#endif

#define MKDIGITS_VERSION    "1.1"
#define DEBUG 1
/*
** private function prototypes
*/
static void extractDigits _Declare ((char *));
static char *myStrtok _Declare ((char *,char *));

static char *ImagePrefix[]=
{
    "zero.gif","one.gif","two.gif","three.gif","four.gif", 
    "five.gif","six.gif","seven.gif","eight.gif","nine.gif",
    "colon.gif","am.gif","pm.gif","comma.gif","dash.gif",
    "d.fig","h.gif","m.gif","s.gif"
};

int main(argc,argv)
int
    argc;
char
    **argv;
{
    int
        rc=0;

    register int
        i;


    if (argc != 2)
    {
        (void) fprintf (stderr,"\nextdgts %s by ma_muquit@fccc.edu\n\n",
            MKDIGITS_VERSION);
        (void) fprintf (stderr,
            "Usage: %s strip.gif\n\n",argv[0]);
        exit(1);
    }
    Gdebug=1;

    extractDigits(argv[1]);

    return (0);
}

/*
** extract the digits from the strip
*/
void extractDigits(strip)
char
    *strip;
{
    Image
        *image,
        *dimage;

    int
        nseg;

    int
        rc=0;

    char
        *p;

    int
        n,
        i;

    char
        *digit;

    RectangleInfo
        rinfo;

    int
        *seg_array=(int *) NULL;

    /* read the strip image */
    image=ReadImage(strip);

    if (image == (Image *) NULL)
    {
        (void) fprintf (stderr,"Unable to read strip image:%s\n",strip);
        exit(1);
    }
    
    if (image->comments != (char *) NULL)
    {
        (void) fprintf (stderr,"%s\n",image->comments);
        rc=sscanf(image->comments,"%d",&nseg);
        if (rc != 1)
        {
            (void) fprintf (stderr,
                "No string segment info found in GIF comment ext.\n");
             exit(1);
        }

        seg_array=(int *) malloc((nseg+2)*sizeof(int));
        if (seg_array == (int *) NULL)
        {
            (void) fprintf (stderr,
                "Memory Allocation Failed for seg_array!\n");
             exit(1);
        }

        p=image->comments;
        i=0;
        while((digit=myStrtok(p,":")) != (char *) NULL)
        {
            p=(char *) NULL;
            if (digit != (char *) NULL)
            {
                seg_array[i] =atoi(digit);
                i++;
            }
        }

        rinfo.y=0;
        rinfo.width=15;
        rinfo.height=image->rows;

        for (n=0; n < nseg; n++)
        {
            rinfo.x=seg_array[n+1];
            rinfo.width=seg_array[n+2]-seg_array[n+1];
            dimage=CropImage(image,&rinfo);
            if (dimage != (Image *) NULL)
            {
                (void) fprintf (stderr,"Writing..%s\n",ImagePrefix[n]);
                WriteGIFImage(dimage,ImagePrefix[n]);
                DestroyAnyImageStruct(&dimage);
            }
        }
    }
    else
    {
        (void) fprintf (stderr,"no comment extension!\n");
        exit(1);
    }

}

/*
** from NetBSD?
*/
static char *myStrtok(s, delim)
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
    {       /* no non-delimiter characters */
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
