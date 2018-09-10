#include "combine.h"
#include "defines.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n C o m p r e s s I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function UncompressImage uncompresses runlength-encoded pixels packets to
%  a rectangular array of pixels.
%
%  The format of the UncompressImage routine is:
%
%      status=UncompressImage(image)
%
%  A description of each parameter follows:
%
%    o status: Function UncompressImage returns True if the image is
%      uncompressed otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
unsigned int UncompressImage(image)
Image
  *image;
{
  register int
    i,
    j,
    length;

  register Runlength
    *p,
    *q;

  Runlength
    *uncompressed_pixels;

  if (image->packets == (image->columns*image->rows))
    return(True);
  /*
    Uncompress runlength-encoded packets.
  */
  uncompressed_pixels=(Runlength *) realloc((char *) image->pixels,
    image->columns*image->rows*sizeof(Runlength));
  if (uncompressed_pixels == (Runlength *) NULL)
    return(False);
  image->pixels=uncompressed_pixels;
  p=image->pixels+image->packets-1;
  q=uncompressed_pixels+image->columns*image->rows-1;
  for (i=0; i < image->packets; i++)
  {
    length=p->length;
    for (j=0; j <= length; j++)
    {
      *q=(*p);
      q->length=0;
      q--;
    }
    p--;
  }
  image->packets=image->columns*image->rows;
  return(True);
}
