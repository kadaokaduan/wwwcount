/*
 *  Modulate ()
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2001/03/19 01:59:51 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      From ImageMagick 
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
 *      muquit@semcor.com   18-Aug-95   first cut
 */

#include "combine.h"
#include "defines.h"

#if __STDC__ || defined(sgi) || defined(_AIX)
void XModulate (RGB *color,unsigned char red,unsigned char green,
    unsigned char blue,int modulate)
#else
void XModulate (color,red,green,blue,modulate)
RGB
    *color;
unsigned char
    red,
    green,
    blue;
int
    modulate;
#endif
{
    switch (modulate)
    {
        case HighlightModulate:
        {        
            color->red=(red*HighlightModulate+
                (unsigned int) (MaxRGB-HighlightModulate)*65535)/MaxRGB;
            color->green=(green*HighlightModulate+
                (unsigned int) (MaxRGB-HighlightModulate)*65535)/MaxRGB;
            color->blue=(blue*HighlightModulate+
                (unsigned int) (MaxRGB-HighlightModulate)*65535)/MaxRGB;
            break;
        }

        case ShadowModulate:
        {
            color->red=(red*ShadowModulate)/MaxRGB;
            color->green=(green*ShadowModulate)/MaxRGB;
            color->blue=(blue*ShadowModulate)/MaxRGB;
            break;
        }
    }   /*switch*/
}

#ifdef TEST
void main(argc,argv)
int
    argc;
char
    **argv;
{
    RGB
        color;

    color.red=69;
    color.green=139;
    color.blue=116;

    Modulate(&color,69,139,116,HighlightModulate);
    (void) fprintf (stderr," R,G,B: %d,%d,%d\n",
        color.red, color.green, color.blue);
    Modulate(&color,69,139,116,ShadowModulate);
    (void) fprintf (stderr," R,G,B: %d,%d,%d\n",
        color.red, color.green, color.blue);
}
#endif
