/*
 *  OpaqueImage () - changes the color of an opaque pixel to the asked color
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
 *      muquit@semcor.com   19-Feb-96   first cut
 */
#include "combine.h"

#if __STDC__ || defined(sgi) || defined(_AIX)
void OpaqueImage (Image *image,
    unsigned char sred,unsigned char sgreen,unsigned char sblue,
    unsigned char pred,unsigned char pgreen,unsigned char pblue)
#else
void OpaqueImage (image,sred,sgreen,sblue,pred,pgreen,pblue)
Image
    *image;
unsigned char
    sred,
    sgreen,
    sblue;
unsigned char
    pred,
    pgreen,
    pblue;
#endif
{
#define DeltaX  16

    register Runlength
        *p;

    register int
        i;

    p=image->pixels;
    switch(image->class)
    {
        case DirectClass:
        {
            for (i=0; i < image->packets; i++)
            {
                if (((int) p->red < (int) (sred+DeltaX)) &&
                    ((int) p->red > (int) (sred-DeltaX)) &&
                    ((int) p->green < (int) (sgreen+DeltaX)) &&
                    ((int) p->green > (int) (sgreen-DeltaX)) &&
                    ((int) p->blue < (int) (sblue+DeltaX)) &&
                    ((int) p->blue > (int) (sblue-DeltaX)))
                {
                    p->red=pred;
                    p->green=pgreen;
                    p->blue=pblue;
                }
               p++;
                
             }
            break;
        }
        case PseudoClass:
        {
            double
                distance_squared,
                min_distance;

            int
                distance;

            register int
                index;

             min_distance=3.0*(MaxRGB+1)*(MaxRGB+1);
             index=0;

             for (i=0; i < image->colors; i++)
             {
                distance=(int) sred-(int) image->colormap[i].red;
                distance_squared=(unsigned int) (distance*distance);
                distance=(int) sgreen-(int) image->colormap[i].green;
                distance_squared+=(unsigned int) (distance*distance);
                distance=(int) sblue-(int) image->colormap[i].blue;
                distance_squared+=(unsigned int) (distance*distance);
                if (distance_squared < min_distance)
                {
                    min_distance=distance_squared;
                    index=i;
                }
             }

             image->colormap[index].red=pred;
             image->colormap[index].green=pgreen;
             image->colormap[index].blue=pblue;
             break;
        }
    }
}
