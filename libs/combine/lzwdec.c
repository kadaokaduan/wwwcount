#include "combine.h"
#include "defines.h"
#include "errcds.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L Z W D e c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function LZWDecodeImage uncompresses an image via LZW-coding.
%
%  The format of the LZWDecodeImage routine is:
%
%      status=LZWDecodeImage(image)
%
%  A description of each parameter follows:
%
%    o status:  Function LZWDecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
int LZWDecodeImage(image)
Image
  *image;
{
#define MaxStackSize  4096
#define NullCode  (-1)

  int
    available,
    clear,
    code_mask,
    code_size,
    end_of_information,
    in_code,
    old_code,
    status;

  register int
    bits,
    code,
    count,
    i;

  register Runlength
    *p;

  register unsigned char
    *c;

  register unsigned int
    datum;

  short
    *prefix;

  unsigned char
    data_size,
    first,
    *packet,
    *pixel_stack,
    *suffix,
    *top_stack;
  int
	rc = 0;

  /*
    Allocate decoder tables.
  */
  packet=(unsigned char *) malloc(256*sizeof(unsigned char));
  prefix=(short *) malloc(MaxStackSize*sizeof(short));
  suffix=(unsigned char *) malloc(MaxStackSize*sizeof(unsigned char));
  pixel_stack=(unsigned char *) malloc((MaxStackSize+1)*sizeof(unsigned char));
  if ((packet == (unsigned char *) NULL) ||
      (prefix == (short *) NULL) ||
      (suffix == (unsigned char *) NULL) ||
      (pixel_stack == (unsigned char *) NULL))
    return(MALLOC_FAILED);
  /*
    Initialize LZW data stream decoder.
  */
  data_size=fgetc(image->fp);
  clear=1 << data_size;
  end_of_information=clear+1;
  available=clear+2;
  old_code=NullCode;
  code_size=data_size+1;
  code_mask=(1 << code_size)-1;
  for (code=0; code < clear; code++)
  {
    prefix[code]=0;
    suffix[code]=code;
  }
  /*
    Decode LZW pixel stream.
  */
  datum=0;
  bits=0;
  c=0;
  count=0;
  first=0;
  top_stack=pixel_stack;
  p=image->pixels;
  for (i=0; i < image->packets; )
  {
    if (top_stack == pixel_stack)
      {
        if (bits < code_size)
          {
            /*
              Load bytes until there is enough bits for a code.
            */
            if (count == 0)
              {
                /*
                  Read a new data block.
                */
                count=ReadDataBlock((char *) packet,image->fp);
                if (count <= 0)
                  break;
                c=packet;
              }
            datum+=(*c) << bits;
            bits+=8;
            c++;
            count--;
            continue;
          }
        /*
          Get the next code.
        */
        code=datum & code_mask;
        datum>>=code_size;
        bits-=code_size;
        /*
          Interpret the code
        */
        if ((code > available) || (code == end_of_information))
          break;
        if (code == clear)
          {
            /*
              Reset decoder.
            */
            code_size=data_size+1;
            code_mask=(1 << code_size)-1;
            available=clear+2;
            old_code=NullCode;
            continue;
          }
        if (old_code == NullCode)
          {
            *top_stack++=suffix[code];
            old_code=code;
            first=code;
            continue;
          }
        in_code=code;
        if (code == available)
          {
            *top_stack++=first;
            code=old_code;
          }
        while (code > clear)
        {
          *top_stack++=suffix[code];
          code=prefix[code];
        }
        first=suffix[code];
        /*
          Add a new string to the string table,
        */
        if (available >= MaxStackSize)
          break;
        *top_stack++=first;
        prefix[available]=old_code;
        suffix[available]=first;
        available++;
        if (((available & code_mask) == 0) && (available < MaxStackSize))
          {
            code_size++;
            code_mask+=available;
          }
        old_code=in_code;
      }
    /*
      Pop a pixel off the pixel stack.
    */
    top_stack--;
    p->index=(unsigned short) *top_stack;
    p->length=0;
    p++;
    i++;
  }
  /*
    Initialize any remaining color packets to a known color.
  */
  status=i == image->packets;
  if (status == False)
  {
	rc=1;
  }
  for ( ; i < image->packets; i++)
  {
    p->index=0;
    p->length=0;
    p++;
  }
  SyncImage(image);
  /*
    Free decoder memory.
  */
  (void) free((char *) pixel_stack);
  (void) free((char *) suffix);
  (void) free((char *) prefix);
  (void) free((char *) packet);
  return(rc);
}
