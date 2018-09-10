# Generated automatically from Makefile.in by configure.
#######
# for unix
# Makefile template for extdgts, program  to extract indidivual images from
# strip.gif (make by mkstrip)
#######
#
# ../libs was missing in 2.4, reported by Heicher" <hexular@concentric.net>
# Oct-21-1997
#
INCLUDES= -I. -I../../libs/combine -I../../src
CC= gcc
DEFS=  -DSTDC_HEADERS=1 -DHAVE_SYS_WAIT_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_STRING_H=1 -DHAVE_FCNTL_H=1 -DHAVE_MEMORY_H=1 -DHAVE_MALLOC_H=1 -DHAVE_UNISTD_H=1 -DHAVE_CTYPE_H=1 -DHAVE_STRINGS_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_SYS_PARAM_H=1 -DHAVE_SYS_FILE_H=1 -DHAVE_STDLIB_H=1 -DHAVE_FLOCK=1 -DHAVE_STRCASECMP=1 -DHAVE_MKTIME=1 -DHAVE_STRFTIME=1 -DHAVE_GMTIME=1 
DEFINES= $(INCLUDES) $(DEFS) -D__USE_FIXED_PROTOTYPES_
CFLAGS= -O $(DEFINES) 
PROGNAME=extdgts

LIBS= ../../libs/combine/libCombine.a

OBJS= extdgts.o

.c.o:
	rm -f $@
	$(CC) $(CFLAGS) -c $*.c

all: $(PROGNAME)

$(PROGNAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROGNAME) $(OBJS) $(LIBS)
	cp $(PROGNAME) ../../bin

clean:
	rm -f *.o $(PROGNAME) core
