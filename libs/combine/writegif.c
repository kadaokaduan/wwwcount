/*
 *	WriteGIF()	-	writes an image in GIF format
 *
 *	RCS:
 *		$Revision: 1.1.1.1 $
 *		$Date: 2001/03/19 01:59:51 $
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		this function writes an image in GIF89a format
 *		GIF is copyrighted trademark of Compuserve Inc.
 *
 *      from ImageMagick
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
 *	08/04/94	muquit 	first cut for ntf2gif
 */

#include "combine.h"
#include "defines.h"
#include "errcds.h"

int WriteGIFImage(gifimage,filename)
Image
    *gifimage;
char
	*filename;
{
	register int
        x,
		i;
	
	unsigned char
		bits_per_pixel,
		c,
        *alpha;

	unsigned int
		colors,
		status;

	FILE
		*fp;

	int
		rc = 0;
	
    register Runlength
        *p;

    setbuf(stdout, NULL);

    if (filename == (char *) NULL)
    	fp = stdout;
    else
    {
        fp=fopen(filename,"wb");
        if (fp == (FILE *) NULL)
        {
            (void) fprintf (stderr,"Failed to open file: %s\n",filename);
            return (-1);
        }
    }
#ifdef SYS_WIN32
    _setmode(_fileno(fp), _O_BINARY);
#endif
	
	colors = 256;
	gifimage->fp = fp;
    alpha=(unsigned char *)NULL;

    if (gifimage->alpha)
    {
        if (!UncompressImage(gifimage))
            return (5);
        
        p=gifimage->pixels;
        alpha=(unsigned char *)
            malloc(gifimage->columns*gifimage->rows*sizeof(unsigned char));
        if (alpha == (unsigned char *) NULL)
        {
		    fprintf (stderr, "WriteGIF: memory allocation failed!\n");
		    rc = MALLOC_FAILED;
		    goto ExitProcessing;
        } 
        else
        {
            for(x=0; x < (gifimage->columns*gifimage->rows); x++)
            {
                alpha[x]=(unsigned char) (p->index & 0xff);
                p++;
            }
            colors--;
        }
    }

    if ((gifimage->class == DirectClass) || (gifimage->colors > colors))
    {
        QuantizeImage(gifimage, colors, 8, False, RGBColorspace, True);
        SyncImage(gifimage);
        if (!UncompressImage(gifimage))
        {
            (void) fprintf (stderr," Failed to uncompress!\n");
            return(5);
        }
    }

	colors = gifimage->colors;

    if (alpha != (unsigned char *) NULL)
    {
        CompressColormap(gifimage);
        colors++;
    }

#ifdef DEBUG
	fprintf (stderr, "GIF image colors: %d\n", colors);
#endif

	for (bits_per_pixel = 1; bits_per_pixel < 8; bits_per_pixel++)
		if ((1 << bits_per_pixel) >= colors)
			break;
	
	/*
	** Write GIF Header
	*/

	(void) fwrite("GIF89a", 1, 6, fp);
    (void) fflush(fp);
	LSBFirstWriteShort (gifimage->columns, fp);
    (void) fflush(fp);
	LSBFirstWriteShort (gifimage->rows, fp);
    (void) fflush(fp);
	c = 0x80;
	c|=(8-1) << 4;
	c|=(bits_per_pixel-1);
	(void) fputc((char) c, fp);
	(void) fputc (0x0, fp);
	(void) fputc (0x0, fp);

	/*
	** write colormap
	*/

	for (i=0; i < gifimage->colors; i++)
	{
		(void) fputc ((char) gifimage->colormap[i].red, fp);
		(void) fputc ((char) gifimage->colormap[i].green, fp);
		(void) fputc ((char) gifimage->colormap[i].blue, fp);
	}

    /*
    ** wrtie transparent channel
    */
    if (alpha != (unsigned char *) NULL)
    {
        p=gifimage->pixels;
        for (x=0; x < (gifimage->columns*gifimage->rows); x++)
        {
            if (alpha[x] == Transparent)
                break;
            p++;
        }
        /* p->index was huge for some images, mm may-08-1999 */
        p--;
        if (p->index > 255 )
            p->index=0;
        (void) fputc((char) gifimage->colormap[p->index].red,fp);
        (void) fputc((char) gifimage->colormap[p->index].green,fp);
        (void) fputc((char) gifimage->colormap[p->index].blue,fp);
        i++;
    }

	for (; i < (int) (1 << bits_per_pixel); i++)
	{
		(void) fputc (0x0, fp);
		(void) fputc (0x0, fp);
		(void) fputc (0x0, fp);
	}

	if (strcmp(gifimage->type, "GIF87") != 0)
	{
		if (gifimage->comments != (char *) NULL)
		{
			register char
				*p;
			
			register unsigned int
				count;
			
			/*
			** write comment extension block
			*/

			(void) fputc (0x21, fp);
			(void) fputc (0xfe, fp);

			p = gifimage->comments;

			while ((int) strlen (p) > 0)
			{
				count = Min ((int) strlen(p), 255);
				(void) fputc (count, fp);

				for (i=0; i < count; i++)
					(void) fputc (*p++, fp);
			}

			(void) fputc (0x0, fp);
		}

        if (alpha != (unsigned char *) NULL)
        {
            (void) fputc(0x21,fp);
            (void) fputc(0xf9,fp);
            (void) fputc(0x4,fp);
            (void) fputc(0x1,fp);
            (void) fputc(0x0,fp);
            (void) fputc(0x0,fp);
            (void) fputc((char) gifimage->colors,fp);
            (void) fputc(0x0,fp);
            
            p=gifimage->pixels;
            for (x=0; x < (gifimage->columns*gifimage->rows); x++)
            {
                if (alpha[x] == Transparent)
                    p->index=gifimage->colors;
                p++;
            } 
            (void) free ((char *) alpha);
        }
	}
/*
** write gifimage separator
*/
	(void) fputc (',', fp);

	LSBFirstWriteShort (0, fp);
    (void) fflush(fp);
	LSBFirstWriteShort (0, fp);
    (void) fflush(fp);
	LSBFirstWriteShort (gifimage->columns, fp);
    (void) fflush(fp);
	LSBFirstWriteShort (gifimage->rows, fp);
    (void) fflush(fp);
	(void) fputc (0x0, fp);
	c = Max(bits_per_pixel, 2);
	(void) fputc ((char) c, fp);

	status = LZWEncodeImage (gifimage, Max(bits_per_pixel, 2)+1);

	if (status == False)
	{
		(void) fprintf (stderr,
        "Unable to write gifimage, Memory allocation failed!\n");
		rc = MALLOC_FAILED;
		goto ExitProcessing;
	}

	(void) fputc(0x0, fp);
	(void) fputc(';',fp);

ExitProcessing:
    if (fp != stdout)
        (void) fclose((FILE *) fp);
	return (rc);
}
