# port to VMS 6.2-> VAX and Alpha by phil@pottsoft.demon.co.uk
# 24-Nov-1997
#
# To build with DEC C:   MMK  ( or MMS)
# To build with GNU C:   MMK/MACRO=(GNU_C=1)  ( or MMS)
##

COMBINE = [-.LIBS.COMBINE]
BIGPLUS = [-.LIBS.BIGPLUS]
##
# do not undefine ACESS_AUTH
##
.ifdef GNU_C
DEFS = /DEFINE=("STDC_HEADERS=1","HAVE_STRING_H=1","HAVE_UNISTD_H=1",\
  "HAVE_FCNTL_H=1","HAVE_MALLOC_H=1","HAVE_MKTIME","ACCESS_AUTH=1","__VMS")
.else
DEFS = /DEFINE=("STDC_HEADERS=1","HAVE_STRING_H=1","HAVE_UNISTD_H=1",\
  "HAVE_FCNTL_H=0","HAVE_MALLOC_H=0","HAVE_MKTIME","ACCESS_AUTH=1")
.endif
INCLUDES = /INCLUDE=([],$(COMBINE),$(BIGPLUS))
LIBS = $(COMBINE)COMBINE.OLB/LIBRARY,$(BIGPLUS)BIGPLUS.OLB/LIBRARY
DEFINES= $(INCLUDES) $(DEFS)
# add /NOOPT/DEBUG to CFLAGS, and /DEBUG to LINK below, for debug version
CFLAGS= /PREFIX=ALL $(DEFINES)

OBJS= main.obj,parse.obj,image.obj,utils.obj,rwdata.obj,llist.obj,setget.obj

#
# make the GIF combining library
#
COUNT_PROG.EXE : $(OBJS),$(LIBS)
        LINK $(OBJS) /EXECUTABLE=COUNT_PROG.EXE,$(LIBS)

$(LIBS) :
        old_default = F$ENVIRONMENT("DEFAULT")
        SET DEFAULT $(COMBINE)
        $(MMS)
        SET DEFAULT 'old_default'
        SET DEFAULT $(BIGPLUS)
        $(MMS)
        SET DEFAULT 'old_default'

MAIN.OBJ  : COUNT.H,CONFIGVMS.h
PARSE.OBJ : COUNT.H,CONFIGVMS.h

CLEAN :
	DELETE *.OBJ;
