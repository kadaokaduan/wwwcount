#ifndef INI_H
#define INI_H
#include <stdio.h>

#if STDC_HEADERS
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#endif

#if HAVE_STRING_H
#include <string.h>
#endif

#if HAVE_MALLOC_H
#include <malloc.h>
#endif

#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif

#ifdef SYS_WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>


#ifdef SYS_WIN32
#include <io.h>
#include <share.h>
#endif


#if __STDC__ || defined(sgi) || defined(_AIX)
#define _Declare(formal_parameters) formal_parameters
#else
#define _Declare(formal_parameters) ()
#define const
#endif

#define     MS_STYLE            1
#define     NOT_MS_STYLE        2

/*
** function prototypes
*/
int paramProcess        _Declare ((char *filename,int style,
                                  int (*sfunc)(char *),
                                  int (*pfunc)(char *,char *)));
char *getCurrentSection _Declare (());

#endif  /* INI_H */
