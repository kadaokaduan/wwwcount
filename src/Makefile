# Generated automatically from Makefile.in by configure.
#####
# Makefile template for Count 2.0
# This file is used by the configure program to generate Makefile
# by Muhammad A Muquit
# 04/10/95, muquit@semcor.com
# http://www.semcor.com/~muquit/
# updated for Count 1.2 06/06/95, --muquit
# updated for Count 1.5 09/17/95, -- muquit
# updated for Count 2.0 11/25/95, -- muquit
#
# $Revision: 1.2 $
# $Date: 2001/04/08 16:15:51 $
#
#####

CFG_INCLUDES= -I../libs/mcfg
BIGPLUS_INCLUDES= -I../libs/bigplus
COMBINE_INCLUDES= -I../libs/combine

INCLUDES= -I. $(CFG_INCLUDES) $(BIGPLUS_INCLUDES) $(COMBINE_INCLUDES)
CC= gcc
DEFS=  -DSTDC_HEADERS=1 -DHAVE_SYS_WAIT_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_STRING_H=1 -DHAVE_FCNTL_H=1 -DHAVE_MEMORY_H=1 -DHAVE_MALLOC_H=1 -DHAVE_UNISTD_H=1 -DHAVE_CTYPE_H=1 -DHAVE_STRINGS_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_SYS_PARAM_H=1 -DHAVE_SYS_FILE_H=1 -DHAVE_STDLIB_H=1 -DHAVE_FLOCK=1 -DHAVE_STRCASECMP=1 -DHAVE_MKTIME=1 -DHAVE_STRFTIME=1 -DHAVE_GMTIME=1 
PROGNAME=Count.cgi

CFG_LIB= ../libs/mcfg/libmcfg.a
BIGPLUS_LIB= ../libs/bigplus/libbigplus.a
IMAGE_LIB= ../libs/combine/libCombine.a
LIBS= $(CFG_LIB) $(BIGPLUS_LIB) $(IMAGE_LIB)  

#
# USE_DNS added by Sean Reifschneider <jafo@tummy.com> 1997-11-21.
# When defined, USE_DNS will cause the referer host to be looked up
# in DNS for checking in the .cfg file.  This allows people running
# virtual hosts to list only their IP addresses instead of having to
# add each virtual domain name to the .cfg file.
#USE_DNS= -DUSE_DNS=1

DEFINES= $(INCLUDES) $(DEFS) $(USE_DNS)
CFLAGS= -g -O2 -Wall $(DEFINES) -DSYS_UNIX=1 -DDEBUG=1


OBJS= main.o parse.o image.o utils.o rwdata.o llist.o setget.o cfg.o wildmat.o

.c.o:
	rm -f $@
	$(CC) $(CFLAGS) -c $*.c

all: $(PROGNAME)

$(PROGNAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROGNAME) $(OBJS) $(LIBS)
	@cp $(PROGNAME) ../bin	        

main.o:	count.h config.h
parse.o: count.h config.h

clean:
	rm -f $(OBJS) Count.cgi core
