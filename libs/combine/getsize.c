/*
 *	GetGIFsize - reads the image size in a GIF file
 *
 *	RCS:
 *		$Revision: 1.1.1.1 $
 *		$Date: 2001/03/19 01:59:51 $
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *      This functions returns after reading the size params in a GIF file
 *
 *	Input Parameters:
 *		type	identifier	description
 *
 *		char    *filename
 *      unsigned int *width;
 *      unsigned int *height;
 *
 *	Output Parameters:
 *		type	identifier	description
 *
 *		unsigned int *width
 *      unsigned int *height
 *
 *	Return Values:
 *		value	description
 *      0 on success
 *
 *	Side Effects:
 *		text
 *
 *	Limitations and Comments:
 *		text
 *
 *	Development History:
 *		when	who		why
 *  02/18/96    muquit  first cut
 */

#include "combine.h"
#include "defines.h"
#include "errcds.h"


int GetGIFsize(filename,width,height)
char
    *filename;

unsigned int
    *width,
    *height;
{

#define BitSet(byte, bit) (((byte) & (bit)) == (bit))
#define LSBFirstOrder(x, y)	(((y) << 8) | (x))

	int
		rc = 0;

    short int
		transparency_index;

	unsigned char
		c,
		*global_colormap,
		header[MaxTextLength],
		type[12];

	unsigned int
		global_colors;

	FILE
		*fp=(FILE *) NULL;
	
    *width=0;
    *height=0;

	fp = fopen (filename, "rb");

	if (fp == (FILE *) NULL)
	{
		fprintf (stderr, "could not open file: %s\n",filename);
		rc = OPEN_FAILED;
		goto ExitProcessing;
	}


	rc = Read_Data ((char *) type, 1, 6, fp);

	if ((rc == 1) || ((strncmp((char *) type, "GIF87", 5) != 0) &&
		(strncmp ((char *) type, "GIF89", 5) != 0)))
	{
		fprintf (stderr, "Not a GIF Image: [%s]\n",filename);
		rc = NOT_A_GIF_FILE;
		goto ExitProcessing;
	}

	rc = Read_Data ((char *) header, 1, 7, fp);

	if (rc == 1)
	{
		rc = FAILED_TO_READ_SCREEND;
		goto ExitProcessing;
	}

	global_colors = 0;
	global_colormap = (unsigned char *) NULL;

	if (BitSet(header[4], 0x80))
	{
		global_colors = 1 << ((header[4] & 0x07) + 1);
        fseek(fp,3*global_colors,1);
	}

	transparency_index = (-1);

	for ( ; ; )
	{
		rc = Read_Data ((char *) &c, 1, 1, fp);

		if (rc == 1)
		{
			rc = 0;
			break;
		}

		if (c == ';')	/* terminator */
			break;
		
		if (c == '!')	/* EXTENSION ! */
		{
			rc = Read_Data ((char *) &c, 1, 1, fp);

			if (rc == 1)
			{
				rc = READ_ERR_EXT_BLOCK;
				goto ExitProcessing;
			}

			switch (c)
			{
				case 0xf9:	/* Transperency */
				{
					while (ReadDataBlock ((char *) header, fp) !=0)
					if ((header[0] * 0x01) == 1)
					{
						transparency_index = header[3];
					}
					break;
				
				}
				case 0xfe:	/* comment extension */
				{
					int
						length;
					
					for ( ; ; )
					{
						length = ReadDataBlock ((char *) header, fp);
						if (length <= 0)
							break;
					}
					break;
				}	

				default:
				{
					while (ReadDataBlock ((char *) header, fp) != 0);
					break;
				}
			}
		}


		if (c != ',')
			continue;


		/*
		** read Header Now
		*/

		rc = Read_Data ((char *) header, 1, 9, fp);

		if (rc == 1)
		{
			rc = READ_ERR_WIDTH_HEIGHT;
			goto ExitProcessing;
		}

		*width = LSBFirstOrder (header[4], header[5]);
		*height = LSBFirstOrder (header[6], header[7]);
        break;
	}/* for (;;) */

        
ExitProcessing:
	if (fp != (FILE *) NULL)
		(void) fclose ((FILE *) fp);
	return (rc);

}
	
