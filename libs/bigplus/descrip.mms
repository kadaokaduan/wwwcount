# BIGPLUS VMS make file for MMS or MMK
# by phil@pottsoft.demon.co.uk
# 24-Nov-1997
#
# To build with DEC C:   MMK  ( or MMS)
# To build with GNU C:   MMK/MACRO=(GNU_C=1)  ( or MMS)

CC= CC/PREFIX=ALL
DEFS=  
INCLUDES= /INCLUDE=([])
DEFINES= $(INCLUDES) $(DEFS) 

# add /NOOPT/DEBUG to CFLAGS, and /DEBUG to LINK below, for debug version
CFLAGS=  $(DEFINES)

OBJS = bigplus.obj,test.obj

all : bigplus.olb, test.exe

test.exe : bigplus.olb
        LINK/EXECUTABLE=test bigplus.olb/LIBRARY/INCLUDE=TEST

bigplus.olb : $(OBJS)
        IF F$SEARCH("bigplus.olb").EQS."" THEN $ LIBRARY/CREATE bigplus.olb
	LIBRARY/INSERT/REPLACE bigplus.olb $(OBJS) 

bigplus.obj : bigplus.h

clean :
	DELETE *.OBJ;
