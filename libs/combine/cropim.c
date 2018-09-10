/*
 *	CropImage()	-	crops an image
 *
 *	RCS:
 *		$Revision: 1.1.1.1 $
 *		$Date: 2001/03/19 01:59:51 $
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		Adapted from ImageMagick
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
 *	08/10/94	muquit	first cut
 */

#include "combine.h"

Image *CropImage(image,crop_info)
Image
  *image;

RectangleInfo
  *crop_info;
{
  Image
    *cropped_image;

  register int
    x,
    y;

  register Runlength
    *p,
    *q;

	/*
	** make a copy of the image
	*/

	cropped_image=DuplicateImage(image,crop_info->width,
		crop_info->height,False);

	if (cropped_image == (Image *) NULL)
    {
		(void) fprintf (stderr,
            "Unable to crop image, Memory allocation failed\n");
		return((Image *) NULL);
    }

	p=image->pixels;
	image->runlength=p->length+1;

	for (x=0; x < (crop_info->y*image->columns+crop_info->x); x++)
		if (image->runlength != 0)
			image->runlength--;
		else
		{
			p++;
			image->runlength=p->length;
		}

	q=cropped_image->pixels;

	for (y=0; y < (cropped_image->rows-1); y++)
	{
		for (x=0; x < cropped_image->columns; x++)
		{
			if (image->runlength != 0)
				image->runlength--;
			else
			{
				p++;
				image->runlength=p->length;
			}
			*q=(*p);
			q->length=0;
			q++;
		}

	for (x=0; x < (image->columns-cropped_image->columns); x++)
		if (image->runlength != 0)
			image->runlength--;
		else
		{
          p++;
          image->runlength=p->length;
        }
	}

	/*
	** dump last scanline
	*/

	for (x=0; x < cropped_image->columns; x++)
	{
		if (image->runlength != 0)
			image->runlength--;
		else
		{
			p++;
			image->runlength=p->length;
		}
		*q=(*p);
		q->length=0;
		q++;
	}
  return(cropped_image);
}
