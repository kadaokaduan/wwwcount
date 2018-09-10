/*
 *	DestroyAnyImageStruct()	-	frees Image Struct
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
 *	6/29/94		muquit	first cut
 *	10/10/94	muquit	pass &anyimage so it can be NULLed properly
 */
#include "combine.h"
#include "defines.h"

void DestroyAnyImageStruct (anyimage)
Image
	**anyimage;
{
		

	if ((*anyimage)->fp != (FILE *) NULL)
	{
		(*anyimage)->fp = (FILE *) NULL;
	}
	if ((*anyimage)->comments != (char *) NULL)
	{
		(void) free ((char *) (*anyimage)->comments);
		(*anyimage)->comments=(char *) NULL;
	}

	if ((*anyimage)->colormap != (RGB *) NULL)
	{
		(void) free ((char *) (*anyimage)->colormap);
		(*anyimage)->colormap = (RGB *)NULL;
	}


	if ((*anyimage)->alpha_color != (RGB *) NULL)
	{
		(void) free ((char *) (*anyimage)->alpha_color);
		(*anyimage)->alpha_color = (RGB *)NULL;
	}

	if ((*anyimage)->signature != (char *) NULL)
	{
		(void) free ((char *) (*anyimage)->signature);
		(*anyimage)->signature=(char *)NULL;
	}

	if ((*anyimage)->pixels != (Runlength *) NULL)
	{
		(void) free ((char *) (*anyimage)->pixels);
		(*anyimage)->pixels=(Runlength *) NULL;
	}


	if (*anyimage != (Image *) NULL)
	{
		(void) free ((char *) *anyimage);
		*anyimage = (Image *) NULL;
	}
}
