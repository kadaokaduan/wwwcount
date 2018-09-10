#include "combine.h"
#include "defines.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     R G B T r a n s f o r m I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure RGBTransformImage converts the reference image from RGB to
%  an alternate colorspace.
%
%  The format of the RGBTransformImage routine is:
%
%      RGBTransformImage(image,colorspace)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o colorspace: An unsigned integer value that indicates which colorspace
%      to transform the image.
%
%
*/
void RGBTransformImage(image,colorspace)
Image
  *image;

unsigned int
  colorspace;
{
#define X 0
#define Y (MaxRGB+1)
#define Z (MaxRGB+1)*2

  long
    tx,
    ty,
    tz,
    *x,
    *y,
    *z;

  register int
    blue,
    green,
    i,
    red;

  register Runlength
    *p;

  register unsigned char
    *range_limit;

  unsigned char
    *range_table;

  if (colorspace == RGBColorspace)
    return;
  /*
    Allocate the tables.
  */
  x=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  y=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  z=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  range_table=(unsigned char *) malloc(3*(MaxRGB+1)*sizeof(unsigned char));
  if ((x == (long *) NULL) || (y == (long *) NULL) ||
      (z == (long *) NULL) || (range_table == (unsigned char *) NULL))
    {
      (void) fprintf (stderr,
        "Unable to transform color space,Memory allocation failed\n");
      return;
    }
  /*
    Pre-compute conversion tables.
  */
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(unsigned char) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  range_limit=range_table+(MaxRGB+1);
  tx=0;
  ty=0;
  tz=0;
  switch (colorspace)
  {
    case GRAYColorspace:
    {
      /*
        Initialize GRAY tables:

          G = 0.29900*R+0.58700*G+0.11400*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29900)*i;
        y[i+X]=UpShifted(0.58700)*i;
        z[i+X]=UpShifted(0.11400)*i;
        x[i+Y]=UpShifted(0.29900)*i;
        y[i+Y]=UpShifted(0.58700)*i;
        z[i+Y]=UpShifted(0.11400)*i;
        x[i+Z]=UpShifted(0.29900)*i;
        y[i+Z]=UpShifted(0.58700)*i;
        z[i+Z]=UpShifted(0.11400)*i;
      }
      break;
    }
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          I1 = 0.33333*R+0.33334*G+0.33333*B
          I2 = 0.50000*R+0.00000*G-0.50000*B
          I3 =-0.25000*R+0.50000*G-0.25000*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.33333)*i;
        y[i+X]=UpShifted(0.33334)*i;
        z[i+X]=UpShifted(0.33333)*i;
        x[i+Y]=UpShifted(0.50000)*i;
        y[i+Y]=0;
        z[i+Y]=(-UpShifted(0.50000))*i;
        x[i+Z]=(-UpShifted(0.25000))*i;
        y[i+Z]=UpShifted(0.50000)*i;
        z[i+Z]=(-UpShifted(0.25000))*i;
      }
      break;
    }

    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          Cb= -0.16864*R-0.33126*G+0.50000*B
          Cr=  0.50000*R-0.41869*G-0.08131*B

        Cb and Cr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29900)*i;
        y[i+X]=UpShifted(0.58700)*i;
        z[i+X]=UpShifted(0.11400)*i;
        x[i+Y]=(-UpShifted(0.16874))*i;
        y[i+Y]=(-UpShifted(0.33126))*i;
        z[i+Y]=UpShifted(0.50000)*i;
        x[i+Z]=UpShifted(0.50000)*i;
        y[i+Z]=(-UpShifted(0.41869))*i;
        z[i+Z]=(-UpShifted(0.08131))*i;
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          Y = 0.29900*R+0.58700*G+0.11400*B
          I = 0.59600*R-0.27400*G-0.32200*B
          Q = 0.21100*R-0.52300*G+0.31200*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29900)*i;
        y[i+X]=UpShifted(0.58700)*i;
        z[i+X]=UpShifted(0.11400)*i;
        x[i+Y]=UpShifted(0.59600)*i;
        y[i+Y]=(-UpShifted(0.27400))*i;
        z[i+Y]=(-UpShifted(0.32200))*i;
        x[i+Z]=UpShifted(0.21100)*i;
        y[i+Z]=(-UpShifted(0.52300))*i;
        z[i+Z]=UpShifted(0.31200)*i;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          U = -0.14740*R-0.28950*G+0.43690*B
          V =  0.61500*R-0.51500*G-0.10000*B

        U and V, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29900)*i;
        y[i+X]=UpShifted(0.58700)*i;
        z[i+X]=UpShifted(0.11400)*i;
        x[i+Y]=(-UpShifted(0.14740))*i;
        y[i+Y]=(-UpShifted(0.28950))*i;
        z[i+Y]=UpShifted(0.43690)*i;
        x[i+Z]=UpShifted(0.61500)*i;
        y[i+Z]=(-UpShifted(0.51500))*i;
        z[i+Z]=(-UpShifted(0.10000))*i;
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize XYZ tables:

          X = 0.49000*R+0.31000*G+0.20000*B
          Y = 0.17700*R+0.81300*G+0.01100*B
          Z = 0.00000*R+0.01000*G+0.99000*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.49000)*i;
        y[i+X]=UpShifted(0.31000)*i;
        z[i+X]=UpShifted(0.20000)*i;
        x[i+Y]=UpShifted(0.17700)*i;
        y[i+Y]=UpShifted(0.81300)*i;
        z[i+Y]=UpShifted(0.01100)*i;
        x[i+Z]=0;
        y[i+Z]=UpShifted(0.01000)*i;
        z[i+Z]=UpShifted(0.99000)*i;
      }
      break;
    }
  }
  /*
    Convert from RGB.
  */
  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Convert DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        red=p->red;
        green=p->green;
        blue=p->blue;
        p->red=range_limit[DownShift(x[red+X]+y[green+X]+z[blue+X]+tx)];
        p->green=range_limit[DownShift(x[red+Y]+y[green+Y]+z[blue+Y]+ty)];
        p->blue=range_limit[DownShift(x[red+Z]+y[green+Z]+z[blue+Z]+tz)];
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < image->colors; i++)
      {
        red=image->colormap[i].red;
        green=image->colormap[i].green;
        blue=image->colormap[i].blue;
        image->colormap[i].red=
          range_limit[DownShift(x[red+X]+y[green+X]+z[blue+X]+tx)];
        image->colormap[i].green=
          range_limit[DownShift(x[red+Y]+y[green+Y]+z[blue+Y]+ty)];
        image->colormap[i].blue=
          range_limit[DownShift(x[red+Z]+y[green+Z]+z[blue+Z]+tz)];
      }
      SyncImage(image);
      break;
    }
  }
  /*
    Free allocated memory.
  */
  (void) free((char *) range_table);
  (void) free((char *) z);
  (void) free((char *) y);
  (void) free((char *) x);
}
