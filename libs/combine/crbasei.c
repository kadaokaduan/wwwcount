/*
 *	CreateBaseImage()	- create the base image for miv default window
 *
 *	RCS:
 *		$Revision: 1.1.1.1 $
 *		$Date: 2001/03/19 01:59:51 $
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		This function creates a white base image.  miv logo and othet small
 *		images will be composited on it
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
 *		who					when		why
 *		muquit@semcor.com	15-Dec-94	first cut
 */

#include "combine.h"
#include "defines.h"

Image *CreateBaseImage(width, height, red, green, blue,class)
unsigned int
	width;

unsigned int
	height;
int
	red,
	green,
	blue;
unsigned int
    class;
{
	Image
		*base_image;

	register Runlength
		*q;

	int
		i;

	/*
	** allocate memory for the base image
	*/

	base_image = AllocateImageStruct();

	if (base_image == (Image *) NULL)
	{
        (void) fprintf (stderr,
            "Unable to create base image!\n");
        return ((Image *) NULL);
	}
	
	(void) strcpy(base_image->filename, "[internal]");
	(void) strcpy(base_image->type, "[internal]");

	base_image->class = class;
	base_image->columns = width;
	base_image->rows = height;
	base_image->packets = base_image->columns*base_image->rows;

	base_image->pixels = (Runlength *)
		malloc(base_image->packets*sizeof(Runlength));

	if (base_image->pixels == (Runlength *) NULL)
	{
        (void) fprintf (stderr,
            "Unable to create base image!\n");
        return ((Image *) NULL);
	}

    q = base_image->pixels;
    switch(class)
    {
        case DirectClass:
        {
            for (i=0; i < base_image->packets; i++)
            {
                q->red = (unsigned char) red;
                q->green = (unsigned char) green;
                q->blue = (unsigned char) blue;
                q->length = 0;
                q->index = 0;
                q++;
            }
            break;
        }

        case PseudoClass:
        {
            base_image->colors=1;
            base_image->colormap = (RGB *)
                malloc(base_image->colors*sizeof(RGB));
            if (base_image->colormap == (RGB *) NULL)
            {
                (void) fprintf (stderr,
                    "Unable to create base image!..malloc failed\n");
                 return ((Image *) NULL);
            }
            base_image->colormap[0].red=(unsigned char) red;
            base_image->colormap[0].green=(unsigned char) green;
            base_image->colormap[0].blue=(unsigned char) blue;

            for (i=0; i < base_image->packets; i++)
            {
                q->red = base_image->colormap[0].red;
                q->green = base_image->colormap[0].green;
                q->blue = base_image->colormap[0].blue;
                q->length = 0;
                q->index = 0;
                q++;
            }
            break;
        }
    }
    return (base_image);
} 

