/*
 *	ReadGIFImage - reads a GIF Image
 *
 *	RCS:
 *		$Revision: 1.1.1.1 $
 *		$Date: 2001/03/19 01:59:51 $
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		Adapted mostly from ImageMagick 3.1
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
 *	6/28/94		muquit	first cut
 */

#include "combine.h"
#include "defines.h"
#include "errcds.h"


int ReadGIFImage (image)
Image
	*image;
{

#define BitSet(byte, bit) (((byte) & (bit)) == (bit))
#define LSBFirstOrder(x, y)	(((y) << 8) | (x))

	int
		rc = 0;

	int
		pass,
		x, y;
	
    short int
		transparency_index;

	register int
		i;

	register Runlength
		*q;
	
	register unsigned char
		*p;
	
	unsigned char
		c,
		*global_colormap,
		header[MaxTextLength],
		type[12];

	unsigned int
		global_colors,
		image_count,
		interlace,
		local_colormap;

	FILE
		*fp=(FILE *) NULL;
	
	Image
		*next_image = (Image *) NULL;

	fp = fopen (image->filename, "rb");

	if (fp == (FILE *) NULL)
	{
		fprintf (stderr, "could not open file: %s\n",image->filename);
		rc = OPEN_FAILED;
		goto ExitProcessing;
	}

	image->fp = fp;

	rc = Read_Data ((char *) type, 1, 6, fp);

	if ((rc == 1) || ((strncmp((char *) type, "GIF87", 5) != 0) &&
		(strncmp ((char *) type, "GIF89", 5) != 0)))
	{
		fprintf (stderr, "Not a GIF Image: [%s]\n",image->filename);
		rc = NOT_A_GIF_FILE;
		goto ExitProcessing;
	}

#ifdef DEBUG
	fprintf (stderr, " File Type: %s\n",type);
#endif
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

		global_colormap = (unsigned char *)
			malloc (3*global_colors*sizeof(unsigned char));

		if (global_colormap == (unsigned char *) NULL)
		{
			rc = MALLOC_FAILED;
			goto ExitProcessing;
		}

		(void) Read_Data ((char *) global_colormap, 3, (int) global_colors,
			fp);
	}

	transparency_index = (-1);
	image_count = 0;

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

					if (image->comments != (char *) NULL)
					{
						image->comments=(char *) realloc((char *) 
							image->comments,
							(strlen(image->comments)+length+1)*
							sizeof(char));
					}
					else
					{
						image->comments = (char *) malloc ((length + 1)*
							sizeof(char));
						if (image->comments != (char *) NULL)
						{
							*image->comments = '\0';
						}
					}

					if (image->comments == (char *) NULL)
					{
						rc = MALLOC_FAILED;
						goto ExitProcessing;
					}
					header[length] = '\0';
					(void) strcat (image->comments, (char *) header);
#ifdef DEBUG
	fprintf (stderr, " GIF: comment: %s\n", image->comments);
#endif
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


		if (image_count != 0)
		{
			if (next_image != (Image *) NULL)
				DestroyAnyImageStruct(&next_image);

			next_image = AllocateImageStruct();
			if (next_image == (Image *)NULL)
			{
				(void) fprintf (stderr,"Unable to Display GIF!\n");
				rc=MALLOC_FAILED;
				goto ExitProcessing;
			}
			(void) strcpy(next_image->filename, image->filename);
			next_image->fp = fp;

			if (image != (Image *) NULL)
				DestroyAnyImageStruct(&image);
			image=next_image;
		}
		image_count++;
		/*
		** can't handle multiple images in one file
		*/

		/*
		** read Header Now
		*/

		rc = Read_Data ((char *) header, 1, 9, fp);

		if (rc == 1)
		{
			rc = READ_ERR_WIDTH_HEIGHT;
			goto ExitProcessing;
		}

		interlace = BitSet (header[8], 0x40);
		local_colormap = BitSet (header[8], 0x80);
		image->columns = LSBFirstOrder (header[4], header[5]);
		image->rows = LSBFirstOrder (header[6], header[7]);
		image->packets = image->columns*image->rows;

		if (image->pixels != (Runlength *) NULL)
		{
			(void) free ((char *) image->pixels);
		}

		image->pixels = (Runlength *)
			malloc (image->packets*sizeof(Runlength));

		if (image->pixels == (Runlength *) NULL)
		{
			rc = MALLOC_FAILED;
			goto ExitProcessing;
		}
		
		image->class = PseudoClass;
		image->colors = 
			!local_colormap ? global_colors : 1 << ((header[8] & 0x07) + 1);
#ifdef DEBUG
	fprintf (stderr, " GIF: no of colors: %d\n", image->colors);
#endif

		image->colormap = (RGB *) malloc (image->colors*sizeof(RGB));

		if (image->colormap == (RGB *) NULL)
		{
			rc = MALLOC_FAILED;
			goto ExitProcessing;
		}

		if (!local_colormap)
		{
			p = global_colormap;

			for (i=0; i < image->colors; i++)
			{
				image->colormap[i].red = (*p++);
				image->colormap[i].green = (*p++);
				image->colormap[i].blue = (*p++);
			}
		}
		else
		{
			unsigned char
				*colormap;

			colormap = (unsigned char *)
				malloc (3*image->colors*sizeof(unsigned char));

			if (colormap == (unsigned char *) NULL)
			{
				rc = MALLOC_FAILED;
				goto ExitProcessing;
			}

			(void) Read_Data ((char *) colormap, 3, (int) image->colors, 
				fp);
			
			p = colormap;

			for (i=0; i < image->colors; i++)
			{
				image->colormap[i].red = (*p++);
				image->colormap[i].green = (*p++);
				image->colormap[i].blue = (*p++);
			}
			(void) free ((char *) colormap);
		}
		rc = LZWDecodeImage (image);
	    if (interlace)
       	{ 
        	Image
          		*interlaced_image;
        	register Runlength
           		*p;

			static int
			interlace_rate[4] = { 8, 8, 4, 2 },
			interlace_start[4] = { 0, 4, 2, 1 };
            
#ifdef DEBUG
    fprintf (stderr, " GIF: processing Interlaced image\n");
#endif
			interlaced_image = DuplicateImage (image, image->columns,
				image->rows, True);

			if (interlaced_image == (Image *) NULL)
			{
				rc = MALLOC_FAILED;
				/* destroy interlaced_image */
				goto ExitProcessing;
			} 
            
			p = interlaced_image->pixels;
			q = image->pixels;

			for (pass=0; pass < 4; pass++)
			{
				y = interlace_start[pass];
                
				while (y < image->rows)
				{
					q = image->pixels + (y*image->columns);
					for (x=0; x < image->columns; x++)
					{
						*q = (*p);
						p++;
						q++;
					}
					y+=interlace_rate[pass];
				}
			}
			DestroyAnyImageStruct (&interlaced_image);
		}/*if interlace*/

            /*
            ** handle transparency
            */

            if (transparency_index >= 0)
            {
                /*
                ** create alpha channel
                */
                q=image->pixels;
                for(i=0; i < image->packets; i++)
                {
                    if (q->index != (unsigned short) transparency_index)
                        q->index=Opaque;
                    else
                        q->index=Transparent;
                    q++;
                }
                transparency_index=(-1);
                image->class=DirectClass;
                image->alpha=True;
            }

			if (rc == 1)
			{
				rc = 0;
                (void) fprintf (stderr," Error\n");
				break;
			}

        break;
	}/* for (;;) */

		if (global_colormap != (unsigned char *) NULL)
		{
			(void) free ((char *) global_colormap);
		}

		if (image->pixels == (Runlength *) NULL)
		{
			rc = CORRUPT_GIF_IMAGE;
			goto ExitProcessing;
		}

        CompressColormap(image);
ExitProcessing:
	if (fp != (FILE *) NULL)
		(void) fclose ((FILE *) fp);
	return (rc);

}
	
