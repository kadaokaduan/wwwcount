/*
 *  SetPixel - sets R,G,B value to a specific location of an image
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
 *      text
 *
 *  Development History:
 *      who                 when        why
 *      muquit@semcor.com   30-Aug-95   fist cut
 */

#include "combine.h"
#include "defines.h"

void SetPixel(image,x,y,font_info,do_what)
Image
    *image;
int
    x,
    y;
SFontInfo
    *font_info;
unsigned int
    do_what;
{
    register Runlength
        *p;

    p=image->pixels+(y*image->columns+x);
    /*
    ** promote image to DirectClass
    */
    if (image->class == PseudoClass)
        image->class=DirectClass;

    switch (image->class)
    {
        case DirectClass:
        {
            if (do_what == 1) /* bg+fg*/
            {
                p->red=font_info->bgr;
                p->green=font_info->bgg;
                p->blue=font_info->bgb;
            }
            else
            {
                p->red=font_info->fgr;
                p->green=font_info->fgg;
                p->blue=font_info->fgb;
            }

        break;
        }

        case PseudoClass:
        {
            register unsigned short
                index;

            index=p->index;

            if (do_what == 1) /* bg+fg*/
            {
                image->colormap[index].red=font_info->bgr;
                image->colormap[index].green=font_info->bgg;
                image->colormap[index].blue=font_info->bgb;
            }
            else
            {
                image->colormap[index].red=font_info->fgr;
                image->colormap[index].green=font_info->fgg;
                image->colormap[index].blue=font_info->fgb;
            }
            SyncImage(image);
            break;
        }
    }
}
