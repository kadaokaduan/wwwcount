#include "combine.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d D a t a B l o c k                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadDataBlock reads data from the image file and returns it.  The
%  amount of data is determined by first reading a count byte.  If
%  ReadDataBlock cannot read the requested number of items, `-1' is returned
%  indicating an error.
%
%  The format of the ReadData routine is:
%
%      status=ReadData(data,file)
%
%  A description of each parameter follows:
%
%    o status:  Function ReadData returns the number of characters read
%      unless there is an error, otherwise `-1'.
%
%    o data:  Specifies an area to place the information reuested from
%      the file.
%
%    o file:  Specifies a file to read the data.
%
%
*/
int ReadDataBlock(data,file)
char
  *data;

FILE
  *file;
{
  unsigned char
    count;

  int
	rc = 0;

  rc = Read_Data((char *) &count,1,1,file);
  if (rc == 1)
    return(rc);
  if (count == 0)
    return(rc);
  rc=Read_Data(data,1,(int) count,file);
  if (rc == 1)
    return(rc);
  return(count);
}
