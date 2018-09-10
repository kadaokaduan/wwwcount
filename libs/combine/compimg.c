#include "combine.h"
#include "defines.h"

void CompressImage(image)
Image
  *image;
{
  register int
    i;

  register Runlength
    *p,
    *q;

  /*
    Compress image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  image->packets=0;
  q=image->pixels;
  q->length=MaxRunlength;

  if (image->alpha)
    for (i=0; i < (image->columns*image->rows); i++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      if ((p->red == q->red) && (p->green == q->green) &&
          (p->blue == q->blue) && (p->index == q->index) &&
          (q->length < MaxRunlength))
        q->length++;
      else
        {
          if (image->packets != 0)
            q++;
          image->packets++;
          *q=(*p);
          q->length=0;
        }
    }
  else
    for (i=0; i < (image->columns*image->rows); i++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      if ((p->red == q->red) && (p->green == q->green) &&
          (p->blue == q->blue) && (q->length < MaxRunlength))
        q->length++;
      else
        {
          if (image->packets != 0)
            q++;
          image->packets++;
          *q=(*p);
          q->length=0;
        }
    }
  image->pixels=(Runlength *) realloc((char *) image->pixels,
    image->packets*sizeof(Runlength));
  /*
    Runlength-encode only if it consumes less memory than no compression.
  */
  if (image->compression == RunlengthEncodedCompression)
    if (image->class == DirectClass)
      {
        if (image->packets >= ((image->columns*image->rows*3) >> 2))
          image->compression=NoCompression;
      }
    else
      if (image->packets >= ((image->columns*image->rows) >> 1))
        image->compression=NoCompression;

}
