/*
** from ImageMagick Version ?
*/

#include "combine.h"

void NegateImage(image)
Image
  *image;
{
  register int
    i;

  register Runlength
    *p;

  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Negate DirectClass packets.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        p->red=(~p->red);
        p->green=(~p->green);
        p->blue=(~p->blue);
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Negate PseudoClass packets.
      */
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=(~image->colormap[i].red);
        image->colormap[i].green=(~image->colormap[i].green);
        image->colormap[i].blue=(~image->colormap[i].blue);
      }
      SyncImage(image);
      break;
    }
  }
}
