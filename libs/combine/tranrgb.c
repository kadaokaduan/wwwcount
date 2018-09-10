/*
 *	TransformRGBImage()	- Converts the reference image from an alternate
 *						  colorspace.
 *
 *	RCS:
 *		$Revision: 1.1.1.1 $
 *		$Date: 2001/03/19 01:59:51 $
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		Adapted from ImageMacgick 3.0
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
 *	4/30/94		mm		first cut
 */

#include "combine.h"
#include "defines.h"


void TransformRGBImage(image,colorspace)
Image
  *image;

unsigned int
  colorspace;
{
#define R 0
#define G (MaxRGB+1)
#define B (MaxRGB+1)*2

  long
    *blue,
    *green,
    *red;

  register int
    i,
    x,
    y,
    z;

  register Runlength
    *p;

  register unsigned char
    *range_limit;

  unsigned char
    *range_table;

  if ((colorspace == RGBColorspace) || (colorspace == GRAYColorspace))
    return;
  /*
    Allocate the tables.
  */
  red=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  green=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  blue=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  range_table=(unsigned char *) malloc(3*(MaxRGB+1)*sizeof(unsigned char));
  if ((red == (long *) NULL) || (green == (long *) NULL) ||
      (blue == (long *) NULL) || (range_table == (unsigned char *) NULL))
    {
      (void) fprintf (stderr,
        "Unable to transform color space,Memory allocation failed\n");
      return;
    }
  /*
    Initialize tables.
  */
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(unsigned char) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  range_limit=range_table+(MaxRGB+1);
  switch (colorspace)
  {
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          R = I1+1.00000*I2-0.66668*I3
          G = I1+0.00000*I2+1.33333*I3
          B = I1-1.00000*I2-0.66668*I3

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=UpShifted(1.0000*0.5)*((i << 1)-MaxRGB);
        blue[i+R]=(-UpShifted(0.66668*0.5))*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=0;
        blue[i+G]=UpShifted(1.33333*0.5)*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=(-UpShifted(1.00000*0.5))*((i << 1)-MaxRGB);
        blue[i+B]=(-UpShifted(0.66668*0.5))*((i << 1)-MaxRGB);
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize XYZ tables:

          R =  1.87597*X-0.53294*Y-0.34303*Z
          G = -0.96670*X+1.99806*Y+0.03136*Z
          B =  0.05735*X-0.11853*Y+1.06118*Z
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.87597)*i;
        green[i+R]=(-UpShifted(0.53294))*i;
        blue[i+R]=(-UpShifted(0.34303))*i;
        red[i+G]=(-UpShifted(0.96670))*i;
        green[i+G]=UpShifted(1.99806)*i;
        blue[i+G]=UpShifted(0.03136)*i;
        red[i+B]=UpShifted(0.05735)*i;
        green[i+B]=(-UpShifted(0.11853))*i;
        blue[i+B]=UpShifted(1.06118)*i;
      }
      break;
    }
    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          R = Y           +1.40200*Cr
          G = Y-0.34414*Cb-0.71414*Cr
          B = Y+1.77200*Cb

        Cb and Cr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.40200*0.5)*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=(-UpShifted(0.34414*0.5))*((i << 1)-MaxRGB);
        blue[i+G]=(-UpShifted(0.71414*0.5))*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=UpShifted(1.77200*0.5)*((i << 1)-MaxRGB);
        blue[i+B]=0;
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          R = 0.97087*Y+1.17782*I+0.59800*Q
          G = 0.97087*Y-0.28626*I-0.72851*Q
          B = 0.97087*Y-1.27870*I+1.72801*Q

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(0.97087)*i;
        green[i+R]=UpShifted(1.17782*0.5)*((i << 1)-MaxRGB);
        blue[i+R]=UpShifted(0.59800*0.5)*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(0.97087)*i;
        green[i+G]=(-UpShifted(0.28626*0.5))*((i << 1)-MaxRGB);
        blue[i+G]=(-UpShifted(0.72851*0.5))*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(0.97087)*i;
        green[i+B]=(-UpShifted(1.27870*0.5))*((i << 1)-MaxRGB);
        blue[i+B]=UpShifted(1.72801*0.5)*((i << 1)-MaxRGB);
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          R = Y          +1.13980*V
          G = Y-0.39380*U-0.58050*V
          B = Y+2.02790*U

        U and V, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.13980*0.5)*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=(-UpShifted(0.39380*0.5))*((i << 1)-MaxRGB);
        blue[i+G]=(-UpShifted(0.58050*0.5))*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=UpShifted(2.02790*0.5)*((i << 1)-MaxRGB);
        blue[i+B]=0;
      }
      break;
    }
  }
  /*
    Convert to RGB.
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
        x=p->red;
        y=p->green;
        z=p->blue;
        p->red=range_limit[DownShift(red[x+R]+green[y+R]+blue[z+R])];
        p->green=range_limit[DownShift(red[x+G]+green[y+G]+blue[z+G])];
        p->blue=range_limit[DownShift(red[x+B]+green[y+B]+blue[z+B])];
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
        x=image->colormap[i].red;
        y=image->colormap[i].green;
        z=image->colormap[i].blue;
        image->colormap[i].red=
          range_limit[DownShift(red[x+R]+green[y+R]+blue[z+R])];
        image->colormap[i].green=
          range_limit[DownShift(red[x+G]+green[y+G]+blue[z+G])];
        image->colormap[i].blue=
          range_limit[DownShift(red[x+B]+green[y+B]+blue[z+B])];
      }
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        x=p->red;
        y=p->green;
        z=p->blue;
        p->red=range_limit[DownShift(red[x+R]+green[y+R]+blue[z+R])];
        p->green=range_limit[DownShift(red[x+G]+green[y+G]+blue[z+G])];
        p->blue=range_limit[DownShift(red[x+B]+green[y+B]+blue[z+B])];
        p++;
      }
      break;
    }
  }
  /*
    Free allocated memory.
  */
  (void) free((char *) range_table);
  (void) free((char *) blue);
  (void) free((char *) green);
  (void) free((char *) red);
}
