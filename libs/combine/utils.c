#include "combine.h"
#include "defines.h"

/* initialize Image comment */
/* from ImageMagick */

void commentImage(image,comments)
Image
    *image;
char
    *comments;
{
    int
        length;

    if (image == (Image *) NULL)
        return;

    if (comments == (char *) NULL)
        return;

    if (*comments == '\0')
        return;

    if (image->comments != (char *) NULL)
        (void) free((char *) image->comments);

    length=strlen(comments) + 8;
    image->comments = (char *) malloc(length);
    if (image->comments != NULL)
    {
        (void) strcpy(image->comments,comments);
    }
    return;
}
