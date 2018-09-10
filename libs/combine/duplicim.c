/*
 *	DuplicateImage()	-	duplicates fields in Image struct
 *
 *	RCS:
 *		$Revision: 1.1.1.1 $
 *		$Date: 2001/03/19 01:59:51 $
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		Adapted from ImageMagick 3.0
 *
 *	Input Parameters:
 *		type	identifier	description
 *
 *		text
 *
 *	Output Parameters:
 *		type	identifier	description
 *
 *		text
 *
 *	Return Values:
 *		value	description
 *
 *	Side Effects:
 *		text
 *
 *	Limitations and Comments:
 *		text
 *
 *	Development History:
 *		when	who		why
 *	5/5/94		mm		first cut
 */

#include "combine.h"
#include "defines.h"


Image *DuplicateImage(image,columns,rows,copy_pixels)
Image
  *image;

unsigned int
  columns,
  rows,
  copy_pixels;
{
  Image
    *copy_image;

  register int
    i;

  /*
  ** Allocate space for Image struct
  */

  copy_image=(Image *) malloc(sizeof(Image));

  if (copy_image == (Image *) NULL)
    return((Image *) NULL);
  *copy_image=(*image);

  if (image->comments != (char *) NULL)
  { 
      copy_image->comments=(char *)
        malloc(((strlen(image->comments)+1)*sizeof(char)));

      if (copy_image->comments == (char *) NULL)
        return((Image *) NULL);

      (void) strcpy(copy_image->comments,image->comments);
   }

  copy_image->columns=columns;
  copy_image->rows=rows;

  if (image->colormap != (RGB *) NULL)
  {
      copy_image->colormap=(RGB *)
        malloc(image->colors*sizeof(RGB));

      if (copy_image->colormap == (RGB *) NULL)
        return((Image *) NULL);

      for (i=0; i < image->colors; i++)
        copy_image->colormap[i]=image->colormap[i];
  }

  if (image->signature != (char *) NULL)
  {
      copy_image->signature=(char *)
        malloc(((strlen(image->signature)+1)*sizeof(char)));

      if (copy_image->signature == (char *) NULL)
        return((Image *) NULL);

      (void) strcpy(copy_image->signature,image->signature);
   }

  if (!copy_pixels)
    copy_image->packets=copy_image->columns*copy_image->rows;

  copy_image->pixels=(Runlength *)
    malloc((unsigned int) copy_image->packets*sizeof(Runlength));

  if (copy_image->pixels == (Runlength *) NULL)
    return((Image *) NULL);

  if (copy_pixels)
  {
      register Runlength
        *p,
        *q;

      if ((image->columns != columns) || (image->rows != rows))
        return((Image *) NULL);

      p=image->pixels;
      q=copy_image->pixels;
      for (i=0; i < image->packets; i++)
      {
        *q=(*p);
        p++;
        q++;
      }
   }
  return(copy_image);
}
