/*
**  test the library
**
**  RCS
**      $Revision: 1.1.1.1 $
**      $Date: 2001/03/19 01:59:51 $
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Nov-23-1997    first cut
*/

#include "combine.h"

int main(int argc,char **argv)
{
    Image
        *image;

    if (argc != 2)
    {
        (void) fprintf(stderr,"usage: %s <image>\n",argv[0]);
        exit(0);
    }

    image=ReadImage(argv[1]);
    if (image == (Image *) NULL)
        exit(1);

    AlphaImage(image,0,255,0);
    WriteGIFImage(image,NULL);

    return(0);
}
