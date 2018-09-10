/*
 *	MrotateImage()	- rotates image an integral of 90 degrees
 *					
 *
 *	RCS:
 *		$Revision: 1.1.1.1 $
 *		$Date: 2001/03/19 01:59:51 $
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		From ImageMagick
 *
 *	Input Parameters:
 *		type	identifier	description
 *
 *		Image   *image      image to rotate
 *      int     degress     degrees to rotate (intergral of 90)
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
 *		rotation is performed on a copy of the passed image. the caller
 *      shuld free the passed copy after obtaing the rotated copy of the
 *      image.
 *
 *	Development History:
 *		when	who		why
 *	07/31/94	muquit	first cut
 */

#include "combine.h"

Image *RotateImage(image, degrees)
Image
	*image;
int
	degrees;
{
	Image
		*rotated_image;

	register Runlength
		*p,
		*q;
			
	register int
		x,
		y;

	unsigned int
		rotations;

    if (degrees%90 != 0)
    {
        (void) fprintf (stderr," %d is not multiple of 90\n",degrees);
        return ((Image *) NULL);
    }

    if (degrees == 360)
    {
        (void) fprintf (stderr," No need to rotate %d\n",degrees);
        return ((Image *) NULL);
    }

	if(degrees < 0)
	{
		degrees+=360;
	}

	for (rotations=0; degrees > 45; rotations++)
	{
		degrees-=90;
	}

	rotations%=4;

	if ((rotations == 1) || (rotations == 3))
	{
		rotated_image=DuplicateImage(image, image->rows, image->columns,
			False);
	}
	else
	{
		rotated_image =DuplicateImage(image, image->columns, image->rows,
			False);
	}

	if (rotated_image == (Image *) NULL)
	{
		(void) fprintf (stderr,
            "Unable to rotate image,Memory allocation failed\n");
		return ((Image *) NULL);
	}

	p = image->pixels;
	image->runlength = p->length+1;

	switch (rotations)
	{
        case 0:
        {
            /*
            ** 0 degree rotation
            */

            q=rotated_image->pixels;
            for (y=0; y < image->rows; y++)
            {
                for (x=0; x < image->columns; x++)
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
            }
            break;
        }

		case 1:	
		{
			/*
			** 90 degrees
			*/
		
			for(x=0; x < rotated_image->columns; x++)
			{
				q = rotated_image->pixels+(rotated_image->columns-x)-1;
				for (y=0; y < rotated_image->rows; y++)
				{
					if (image->runlength != 0)
						image->runlength--;
					else
					{
						p++;
						image->runlength = p->length;
					}

				*q = (*p);
				q->length = 0;
				q+=rotated_image->columns;
				}
			}
			break;
		}

        case 2:
        {
            /*
            ** rotate 180 degrees
            */
           
            q=rotated_image->pixels+
                (rotated_image->columns*rotated_image->rows)-1;
            for (y=image->rows-1; y >=0; y--)
            {
                for (x=0; x < image->columns; x++)
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
                    q--;
                }
            }
            break;
        }
		case 3:
		{
			/*
			** 270 degress
			*/

			for(x=rotated_image->columns-1; x >=0; x--)
			{
				q = rotated_image->pixels+(rotated_image->columns*
					rotated_image->rows)-x-1;
				for (y=0; y < rotated_image->rows; y++)
				{
					if (image->runlength != 0)
						image->runlength--;
					else
					{
						p++;
						image->runlength = p->length;
					}
				*q = (*p);
				q->length = 0;
				q-=rotated_image->columns;
				}
			}
			break;
		}
	}

return(rotated_image);
}
