/*
 *  FrameImage - wraps image with an ornamental frame
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2001/03/19 01:59:51 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      From ImageMagick 3.6.5
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

Image *FrameImage (mimage, frame_info)
Image
    *mimage;
FrameInfo
    *frame_info;
{
    Image
        *framed_image;

    int
        height,
        width;

    register int
        x,
        y;

    register Runlength
        *p,
        *q;

    Runlength
        highlight,
        matte,
        shadow;

    unsigned int
        bevel_width;

    if ((frame_info->outer_bevel < 0) || (frame_info->inner_bevel < 0))
    {
        (void) fprintf (stderr,"Unable to Frame image!\n");
        return ((Image *) NULL);
    }
    
    bevel_width=frame_info->outer_bevel+frame_info->inner_bevel;
    width=(int) frame_info->width-frame_info->x-bevel_width;
    height=(int) frame_info->height-frame_info->y-bevel_width;

    if ((width < mimage->columns) || (height < mimage->rows))
    {
        (void) fprintf (stderr,"Unable to Frame image!\n");
        return ((Image *) NULL);
    }
    
    framed_image=DuplicateImage(mimage,frame_info->width,frame_info->height,
        False);
    if (framed_image == (Image *) NULL)
    {
        (void) fprintf (stderr,"Unable to Frame image!\n");
        return ((Image *) NULL);
    }

    mimage->class=DirectClass;

    matte.red=frame_info->matte_color.red;
    matte.green=frame_info->matte_color.green;
    matte.blue=frame_info->matte_color.blue;
    matte.index=MaxRGB;
    matte.length=0;

    highlight.red=frame_info->highlight_color.red;
    highlight.green=frame_info->highlight_color.green;
    highlight.blue=frame_info->highlight_color.blue;
    highlight.index=MaxRGB;
    highlight.length=0;

    shadow.red=frame_info->shadow_color.red;
    shadow.green=frame_info->shadow_color.green;
    shadow.blue=frame_info->shadow_color.blue;
    shadow.index=MaxRGB;
    shadow.length=0;

    q=framed_image->pixels;
    for (y=0; y < frame_info->outer_bevel; y++)
    {
        for (x=0; x < (framed_image->columns-y); x++)
        {
            *q++=highlight;
        }
        for ( ; x < framed_image->columns; x++)
            *q++=shadow;
    }

    for (y=0; y < (frame_info->y-bevel_width); y++)
    {
        for (x=0; x < frame_info->outer_bevel; x++)
            *q++=highlight;
        for (x=0; x < (framed_image->columns-
            (frame_info->outer_bevel << 1)); x++)
            *q++=matte;
        for (x=0; x < frame_info->outer_bevel; x++)
            *q++=shadow;
    }

    for (y=0; y < frame_info->inner_bevel; y++)
    {
        for (x=0; x < frame_info->outer_bevel; x++)
            *q++=highlight;
        for (x=0; x < (frame_info->x-bevel_width); x++)
            *q++=matte;
        for (x=0; x < (mimage->columns+(frame_info->inner_bevel << 1)-y); x++)
            *q++=shadow;
        for ( ; x < (mimage->columns+(frame_info->inner_bevel << 1)); x++)
            *q++=highlight;
        width=frame_info->width-frame_info->x-mimage->columns-bevel_width;
        for (x=0; x < width; x++)
            *q++=matte;
        for (x=0; x < frame_info->outer_bevel; x++)
            *q++=shadow;
    }
    p=mimage->pixels;
    mimage->runlength=p->length+1;
    for (y=0; y < mimage->rows; y++)
    {
        for (x=0; x < frame_info->outer_bevel; x++)
            *q++=highlight;
        for (x=0; x < (frame_info->x-bevel_width); x++)
            *q++=matte;
        for (x=0; x < frame_info->inner_bevel; x++)
            *q++=shadow;

        /*
        ** Transfer scanline
        */
        for (x=0; x < mimage->columns; x++)
        {
            if (mimage->runlength != 0)
                mimage->runlength--;
            else
            {
                p++;
                mimage->runlength=p->length;
            }
            *q=(*p);
            q->length=0;
            q++;
        }
        for (x=0; x < frame_info->inner_bevel; x++)
            *q++=highlight;
        width=frame_info->width-frame_info->x-mimage->columns-bevel_width;
        for (x=0; x < width; x++)
            *q++=matte;
        for (x=0; x < frame_info->outer_bevel; x++)
            *q++=shadow;
    }
    for (y=frame_info->inner_bevel-1; y >= 0; y--)
    {
        for (x=0; x < frame_info->outer_bevel; x++)
            *q++=highlight;
        for (x=0; x < (frame_info->x-bevel_width); x++)
            *q++=matte;
        for (x=0; x < y; x++)
            *q++=shadow;
        for ( ; x < (mimage->columns+(frame_info->inner_bevel << 1)); x++)
            *q++=highlight;
        width=frame_info->width-frame_info->x-mimage->columns-bevel_width;
        for (x=0; x < width; x++)
            *q++=matte;
        for (x=0; x < frame_info->outer_bevel; x++)
            *q++=shadow;
    }
    height=frame_info->height-frame_info->y-mimage->rows-bevel_width;
    for (y=0; y < height; y++)
    {
        for (x=0; x < frame_info->outer_bevel; x++)
            *q++=highlight;
        for (x=0; x < (framed_image->columns-
            (frame_info->outer_bevel << 1)); x++)
            *q++=matte;
        for (x=0; x < frame_info->outer_bevel; x++)
            *q++=shadow;
    }
    for (y=frame_info->outer_bevel-1; y >= 0; y--)
    {
        for (x=0; x < y; x++)
            *q++=highlight;
        for ( ; x < framed_image->columns; x++)
            *q++=shadow;
    }
return (framed_image);
}
