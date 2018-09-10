/*
 *  mkstrip - program to make a image strip out of several gif images
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2001/03/19 01:59:51 $
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
 *      This program keeps info about the strips in the GIF comment
 *      extension, which is used by the counter program.
 *
 *  Development History:
 *      who                 when        why
 *      muquit@semcor.com   18-Mar-96   first cut
 */


#include "combine.h"
#include "count.h"

void Usage _Declare ((void));
void Debug _Declare ((char *));
void MakeStripImage _Declare ((char **,int));

char
    *progname;

#define MKSTRIP_VERSION "1.0"
#define DEBUG   1

int main(argc,argv)
int
    argc;
char
    *argv[];
{
    int
        i;

    char
        tmpbuf[2048];

    *tmpbuf='\0';
    progname=argv[0];
    if (argc < 2)
    {
        Usage();
    }

    MakeStripImage(argv,argc);
    return (0);
}

void Usage()
{
    (void) fprintf (stderr,"mkstrip Version %s\n\n", MKSTRIP_VERSION);
    (void) fprintf (stderr,
        "Usage: %s image1.gif image2.gif image3.gif ... > final.gif\n\n", progname);
    exit(1);
}

void Debug(str)
char
    *str;
{
#ifdef DEBUG
    (void) fprintf (stderr,"%s\n",str);
#endif
    return;
}

void MakeStripImage(files,n)
char
    **files;
int
    n;
{
    int
        i;
    
    int
        rc;

    unsigned int
        w,
        h,
        bw,
        bh;

    Image
        *base_image,
        *sub_image;

    int
        do_comment,
        count,
        segment;

    w=h=bw=bh=0;
    count=0;
    segment=0;
    do_comment=0;

    for (i=1; i < n; i++)
    {
        rc=GetGIFsize(files[i],&w,&h);
        if (rc)
        {
            (void) fprintf (stderr,
                " Unable to determine digit imaeg size for: %s\n",files[i]);
            exit(1);
        }
        bw += w;
        if (h > bh)
            bh=h;
    }


    base_image=CreateBaseImage (bw,bh,0,0,0,DirectClass);
    /*
    ** add comment to the image. this is important. we will determine
    ** the number of digit images by reading it
    */
    base_image->comments=(char *) malloc(100*sizeof(char));
    if (base_image != (Image *) NULL)
        do_comment=1;

    if (do_comment == 1)
    {
        (void) sprintf(base_image->comments,"%d",(n-1));
    }
    if (base_image == (Image *) NULL)
    {
        (void) fprintf (stderr," Unable to create base image!\n");
        exit(1);
    }

    bw=bh=0;
    for (i=1; i < n; i++)
    {
        sub_image=ReadImage(files[i]);
        if (sub_image != (Image *) NULL)
        {
            FlattenImage(base_image,sub_image,ReplaceCompositeOp,bw,0);
            if (do_comment == 1)
                (void) sprintf(base_image->comments,
                     "%s:%d",base_image->comments,bw);
            bw += sub_image->columns;
            DestroyAnyImageStruct(&sub_image);
        }
        else
        {
            (void) fprintf (stderr,"Failed to create image strip!\n");
            exit(1);
        }
    }
    if (do_comment == 1)
       (void) sprintf(base_image->comments,
             "%s:%d",base_image->comments,bw);
    WriteGIFImage(base_image,(char *) NULL);
}
