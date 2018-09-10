# COMBINE VMS make file for MMS or MMK
# by phil@pottsoft.demon.co.uk
# 24-Nov-1997
#
# To build with DEC C:   MMK  ( or MMS)
# To build with GNU C:   MMK/MACRO=(GNU_C=1)  ( or MMS)

CC= CC/PREFIX=ALL
.ifdef GNU_C
DEFS=  /DEFINE=("STRICT_MODE=1","STDC_HEADERS=1","HAVE_STRING_H=1",\
 "HAVE_FCNTL_H=1","HAVE_MALLOC_H=1","HAVE_SYS_FILE_H=1","HAVE_UNISTD_H=1",\
 "HAVE_FLOCK=0","__VMS")
.else
DEFS=  /DEFINE=("STRICT_MODE=1","STDC_HEADERS=1","HAVE_STRING_H=1",\
 "HAVE_FCNTL_H=0","HAVE_MALLOC_H=0","HAVE_SYS_FILE_H=1","HAVE_UNISTD_H=1",\
 "HAVE_FLOCK=0")
.endif

INCLUDES= /INCLUDE=([])
DEFINES= $(INCLUDES) $(DEFS) 

# add /NOOPT/DEBUG to CFLAGS, and /DEBUG to LINK below, for debug version
CFLAGS=  $(DEFINES)

OBJS = crbasei.obj,allocim.obj,readgif.obj,rddata.obj,dstraimg.obj,\
       readblk.obj,lzwdec.obj,syncimg.obj,duplicim.obj,writegif.obj,\
       lsbfwrs.obj,lzwenc.obj,uncmpimg.obj,readim.obj,flatten.obj,\
       signature.obj,quantize.obj,compimg.obj,rgbtran.obj,tranrgb.obj,\
       compcmap.obj,frameim.obj,modulate.obj,alphaim.obj,setpixel.obj,\
       getfinfo.obj,imgstr.obj,gdfonts.obj,negate.obj,rotate.obj,getsize.obj,\
       opaqim.obj,cropim.obj,testlib.obj

all : combine.olb, testlib.exe

testlib.exe : combine.olb
        LINK/EXECUTABLE=testlib combine.olb/LIBRARY/INCLUDE=TESTLIB

combine.olb : $(OBJS)
        IF F$SEARCH("combine.olb").EQS."" THEN $ LIBRARY/CREATE combine.olb
	LIBRARY/INSERT/REPLACE combine.olb $(OBJS) 

combine.obj : combine.h

clean :
	DELETE *.OBJ;
