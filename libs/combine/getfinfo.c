/*
 *  GetFrameInfo - initializes FrameInfo structure
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
 *      muquit@semcor.com   30-Aug-95   first cut
 */

#include "combine.h"

void GetFrameInfo (im_width, im_height,frame_info)
unsigned int
    im_width,
    im_height;
FrameInfo
    *frame_info;
{
    RGB
        color,
        matte_color;
    frame_info->width=3;
    frame_info->height=3;
    frame_info->outer_bevel=(frame_info->width >> 2)+1;
    frame_info->inner_bevel=frame_info->outer_bevel;
    frame_info->matte_color.red=100;
    frame_info->matte_color.green=139;
    frame_info->matte_color.blue=216;
    frame_info->x=frame_info->width;
    frame_info->y=frame_info->height;
    frame_info->width=im_width+(frame_info->width << 1);
    frame_info->height=im_height+(frame_info->height << 1);


    matte_color.red=frame_info->matte_color.red;
    matte_color.green=frame_info->matte_color.green;
    matte_color.blue=frame_info->matte_color.blue;

    XModulate(&color,matte_color.red,matte_color.green,
        matte_color.blue, HighlightModulate);

    frame_info->highlight_color.red=color.red;
    frame_info->highlight_color.green=color.green;
    frame_info->highlight_color.blue=color.blue;

    XModulate(&color,matte_color.red,matte_color.green,
        matte_color.blue, ShadowModulate);

    frame_info->shadow_color.red=color.red;
    frame_info->shadow_color.green=color.green;
    frame_info->shadow_color.blue=color.blue;

}
