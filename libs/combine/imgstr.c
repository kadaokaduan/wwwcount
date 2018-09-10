/*
 *  String drawing routines
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2001/03/19 01:59:51 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      Adapted from gd 1.2 by TB
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
 *      muquit@semcor.com   30-Aug-95   first cut
 */

#include "combine.h"
#include "gdfonts.h"

void ImageChar(image,f,x,y,c,font_info)
Image
    *image;
gdFontPtr
    f;
int
    x,
    y;
char
    c;
SFontInfo
    *font_info;
{
    int
        cx,
        cy,
        px,
        py;

    int
        fline;

    cx=0;
    cy=0;

    if ((c < f->offset) || (c >= (f->offset + f->nchars)))
        return;

    fline=(c - f->offset) * f->h * f->w;
    for (py = y; (py < (y + f->h)); py++)
    {
        for (px = x; (px < (x + f->w)); px++)
        {
            if (font_info->do_bg == True)
            {
                if (f->data[fline + cy * f->w + cx])
                    SetPixel(image,px,py,font_info,0);
                else
                    SetPixel(image,px,py,font_info,1);
            }
            else
            {
                if (f->data[fline + cy * f->w + cx])
                    SetPixel(image,px,py,font_info,0);
            }                
            cx++;
        }
        cx=0;
        cy++;
    }
}

void ImageString(image,f,x,y,s,font_info)
Image
    *image;
gdFontPtr
    f;
int
    x,
    y;
char
    *s;
SFontInfo
    *font_info;
{
    int
        i,
        l;

    l=(int) strlen(s);

    for (i=0; i < l; i++)
    {
        ImageChar(image,f,x,y,s[i],font_info);
        x += f->w;
    }
}


