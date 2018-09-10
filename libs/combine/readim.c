/*
 *  ReadImge
 *
 *  RCS:
 *      $Revision: 1.1.1.1 $
 *      $Date: 2001/03/19 01:59:51 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      text
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Return Values:
 *      value   description
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      text
 *
 *  Development History:
 *      who                 when        why
 *      muquit@semcor.com   11-Jul-95   first cut
 */

#include "combine.h"

Image *ReadImage (filename)
char
    *filename;
{
    FILE
        *fp = (FILE *) NULL;

    Image
        *image = (Image *) NULL;

    char
        type[12];

    int
        rc = 0;

#ifdef DEBUG
    (void) fprintf (stderr," File: %s\n", filename);
#endif

    *type = '\0';

    fp = fopen (filename, "rb");
    if (fp == (FILE *) NULL)
    {
        (void) fprintf (stderr,
            "Unable to open file: %s\n", filename);
        return ((Image *) NULL);
    }
    
    (void) strcpy (type, "unk");
    (void) Read_Data (type, sizeof(char), sizeof(type), fp);
    (void) fclose (fp);

#ifdef DEBUG
    (void) fprintf (stderr," Format: %s\n", type);
#endif
    if (*type != 'u')
    {
        if (*type == 'G')
        {
            image = AllocateImageStruct ();
            if (image == (Image *) NULL)
            {
                (void) fprintf (stderr,
                    "Malloc failed: AllocateImageStruct()!\n");
                return ((Image *) NULL);
            }
            (void) strcpy (image->type, "GIF");
            (void) strcpy (image->filename, filename);

            rc = ReadGIFImage (image);
            if (rc)
            {
                DestroyAnyImageStruct (&image);
                (void) fprintf (stderr,
                    "Failed to read GIF image: %s\n", filename);
                return ((Image *) NULL);
            }
            return (image);

        }
        else
        {
            (void) fprintf (stderr,"Not a GIF image!\n");
            return ((Image *) NULL);
        }
    }
    else
    {
        (void) fprintf (stderr,"Not a GIF image!\n");
        return ((Image *) NULL);
    }
}
