/*
 *	LZWEncodeImage()	-	description
 *
 *	RCS:
 *		$Revision: 1.1.1.1 $
 *		$Date: 2001/03/19 01:59:51 $
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		Taken from ImageMagick 3.3
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
 */
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L Z W E n c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function LZWEncodeImage compresses an image via LZW-coding.
%
%  The format of the LZWEncodeImage routine is:
%
%      status=LZWEncodeImage(image,data_size)
%
%  A description of each parameter follows:
%
%    o status:  Function LZWEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/

#include "combine.h"
#include "defines.h"

unsigned int LZWEncodeImage(image,data_size)
Image
  *image;

unsigned int
  data_size;
{
#define MaxCode(number_bits)  ((1 << (number_bits))-1)
#define MaxHashTable  5003
#define MaxLZWBits  12
#define MaxLZWTable  (1 << MaxLZWBits)
#define LZWOutputCode(code) \
{ \
  /*  \
    Emit a code. \
  */ \
  if (bits > 0) \
    datum|=((long) code << bits); \
  else \
    datum=(long) code; \
  bits+=number_bits; \
  while (bits >= 8)  \
  { \
    /*  \
      Add a character to current packet. \
    */ \
    packet[byte_count++]=(unsigned char) (datum & 0xff); \
    if (byte_count >= 254) \
      { \
        (void) fputc(byte_count,image->fp); \
        (void) fwrite((char *) packet,1,byte_count,image->fp); \
        byte_count=0; \
      } \
    datum>>=8; \
    bits-=8; \
  } \
  if (free_code > max_code)  \
    { \
      number_bits++; \
      if (number_bits == MaxLZWBits) \
        max_code=MaxLZWTable; \
      else \
        max_code=MaxCode(number_bits); \
    } \
}

  int
    bits,
    byte_count,
    next_pixel,
    number_bits;

  long
    datum;

  register int
    displacement,
    i,
    j;

  register Runlength
    *p;

  short
    clear_code,
    end_of_information_code,
    free_code,
    *hash_code,
    *hash_prefix,
    index,
    max_code,
    waiting_code;

  unsigned char
    *packet,
    *hash_suffix;

  /*
    Uncompress image.
  */
  if (!UncompressImage(image))
    return(False);
  /*
    Allocate encoder tables.
  */
  packet=(unsigned char *) malloc(256*sizeof(unsigned char));
  hash_code=(short *) malloc(MaxHashTable*sizeof(short));
  hash_prefix=(short *) malloc(MaxHashTable*sizeof(short));
  hash_suffix=(unsigned char *) malloc(MaxHashTable*sizeof(unsigned char));
  if ((packet == (unsigned char *) NULL) || (hash_code == (short *) NULL) ||
      (hash_prefix == (short *) NULL) ||
      (hash_suffix == (unsigned char *) NULL))
    return(False);
  /*
    Initialize LZW encoder.
  */
  number_bits=data_size;
  max_code=MaxCode(number_bits);
  clear_code=((short) 1 << (data_size-1));
  end_of_information_code=clear_code+1;
  free_code=clear_code+2;
  byte_count=0;
  datum=0;
  bits=0;
  for (i=0; i < MaxHashTable; i++)
    hash_code[i]=0;
  LZWOutputCode(clear_code);
  /*
    Encode pixels.
  */
  p=image->pixels;
  waiting_code=p->index;
  for (i=1; i < (image->columns*image->rows); i++)
  {
    /*
      Probe hash table.
    */
    p++;
    index=p->index & 0xff;
    j=(int) ((int) index << (MaxLZWBits-8))+waiting_code;
    if (j >= MaxHashTable)
      j-=MaxHashTable;

#ifdef HAS_LZW
    if (hash_code[j] > 0)
      {
        if ((hash_prefix[j] == waiting_code) && (hash_suffix[j] == index))
          {
            waiting_code=hash_code[j];
            continue;
          }
        if (j == 0)
          displacement=1;
        else
          displacement=MaxHashTable-j;
        next_pixel=False;
        for ( ; ; )
        {
          j-=displacement;
          if (j < 0)
            j+=MaxHashTable;
          if (hash_code[j] == 0)
            break;
          if ((hash_prefix[j] == waiting_code) && (hash_suffix[j] == index))
            {
              waiting_code=hash_code[j];
              next_pixel=True;
              break;
            }
        }
        if (next_pixel == True)
          continue;
      }
#endif /* HAS_LZW */
    LZWOutputCode(waiting_code);
    if (free_code < MaxLZWTable)
      {
        hash_code[j]=free_code++;
        hash_prefix[j]=waiting_code;
        hash_suffix[j]=index;
      }
    else
      {
        /*
          Fill the hash table with empty entries.
        */
        for (j=0; j < MaxHashTable; j++)
          hash_code[j]=0;
        /*
          Reset compressor and issue a clear code.
        */
        free_code=clear_code+2;
        LZWOutputCode(clear_code);
        number_bits=data_size;
        max_code=MaxCode(number_bits);
      }
    waiting_code=index;
  }
  /*
    Flush out the buffered code.
  */
  LZWOutputCode(waiting_code);
  LZWOutputCode(end_of_information_code);
  if (bits > 0)
    {
      /*
        Add a character to current packet.
      */
      packet[byte_count++]=(unsigned char) (datum & 0xff);
      if (byte_count >= 254)
        {
          (void) fputc(byte_count,image->fp);
          (void) fwrite((char *) packet,1,byte_count,image->fp);
          byte_count=0;
        }
    }
  /*
    Flush accumulated data.
  */
  if (byte_count > 0)
    {
      (void) fputc(byte_count,image->fp);
      (void) fwrite((char *) packet,1,byte_count,image->fp);
    }
  /*
    Free encoder memory.
  */
  (void) free((char *) hash_suffix);
  (void) free((char *) hash_prefix);
  (void) free((char *) hash_code);
  (void) free((char *) packet);
  if (i < image->packets)
    return(False);
  return(True);
}
