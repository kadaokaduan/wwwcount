/*
 *	AllocateImageStruct() - allocates Image struct and initializes each
 *		member
 *
 *	RCS:
 *		$Revision: 1.1.1.1 $
 *		$Date: 2001/03/19 01:59:51 $
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		text
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
 *	3/31/94		mm		first cut
 */

#include "combine.h"
#include "defines.h"

Image *AllocateImageStruct ()
{
	Image *alloc_image;

	alloc_image = (Image *) malloc (sizeof(Image));

	if (alloc_image == (Image *) NULL)
	{
		fprintf (stderr, "Unable to allocate memory for Image struct\n");
		return ((Image *) NULL);
	}
	*alloc_image->filename = '\0';
	alloc_image->fp = (FILE *) NULL;
	(void) strcpy (alloc_image->type, "unknown");
	alloc_image->comments = (char *) NULL;
	alloc_image->id = UndefinedId;
	alloc_image->class = DirectClass;
	alloc_image->alpha = False;
	alloc_image->compression = RunlengthEncodedCompression;
	alloc_image->columns = 0;
	alloc_image->rows = 0;
	alloc_image->xorig = 0;
	alloc_image->yorig = 0;
	alloc_image->scene = 0;
	alloc_image->colors = 0;
    alloc_image->alpha_color=(RGB *) NULL;
	alloc_image->colormap = (RGB *) NULL;
	alloc_image->signature = (char *) NULL;
	alloc_image->pixels = (Runlength *) NULL;
	alloc_image->packets = 0;
	alloc_image->pack_siz = 0;
	alloc_image->runlength = 0;

	return (alloc_image);
}
