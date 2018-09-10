/*
 *  AlphaImage () - creates an alpha channel
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
 *      muquit@semcor.com   24-Aug-95   first cut
 *      ma_muquit@fccc.edu  Nov-23-1997 modified based on IM 3.9.2
 */
#include "combine.h"

#define COLOR_MATCH(color,target,delta) \
 ((((int) ((color).red)-delta) <= (int) ((target).red)) && \
  ((int) ((target).red) <= ((int) ((color).red)+delta)) && \
  (((int) ((color).green)-delta) <= (int) ((target).green)) && \
  ((int) ((target).green) <= ((int) ((color).green)+delta)) && \
  (((int) ((color).blue)-delta) <= (int) ((target).blue)) && \
  ((int) ((target).blue) <= ((int) ((color).blue)+delta)))

#if __STDC__ || defined(sgi) || defined(_AIX)
void AlphaImage (Image *image,
    unsigned char red,unsigned char green,unsigned char blue)
#else
void AlphaImage (image,red,green,blue)
Image
    *image;
unsigned char
    red,
    green,
    blue;
#endif
{

    register Runlength
        *p;

    register int
        i;
    RGB
        target;

    target.red=red;
    target.green=green;
    target.blue=blue;

    if (!image->alpha)
    {
        /*
        ** initialize alpha channel to opaque
        */
        image->class=DirectClass;
        image->alpha=True;
        p=image->pixels;
        for (i=0; i < image->packets; i++)
        {
            p->index=Opaque;
            p++;
        }
    }
    p=image->pixels;
    for (i=0; i < image->packets; i++)
    {
        if (COLOR_MATCH(*p,target,0))
            p->index=Transparent;
        p++;
    }
}
