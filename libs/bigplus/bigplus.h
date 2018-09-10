#ifndef BIGPLUS_H
#define BIGPLUH_H

/* number.h: Arbitrary precision numbers header file. */

/*  This file is part of GNU bc.
    Copyright (C) 1991, 1992, 1993, 1994, 1997 Free Software Foundation, Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to
    the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

    You may contact the author by:
       e-mail:  phil@cs.wwu.edu
      us-mail:  Philip A. Nelson
                Computer Science Department, 9062
                Western Washington University
                Bellingham, WA 98226-9062
       
*************************************************************************/

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

/* Define the _PROTOTYPE macro if it is needed. */

#ifndef _PROTOTYPE
#ifdef __STDC__
#define _PROTOTYPE(func, args) func args
#else
#define _PROTOTYPE(func, args) func()
#endif
#endif


typedef enum {PLUS, MINUS} sign;

typedef struct
    {
      sign n_sign;
      int  n_len;	/* The number of digits before the decimal point. */
      int  n_scale;	/* The number of digits after the decimal point. */
      int  n_refs;      /* The number of pointers to this number. */
      char n_value[1];  /* The storage. Not zero char terminated. It is 
      			   allocated with all other fields.  */
    } bc_struct;

typedef bc_struct *bc_num;

/* The base used in storing the numbers in n_value above.
   Currently this MUST be 10. */

#define BASE 10

/*  Some useful macros and constants. */

#define CH_VAL(c)     (c - '0')
#define BCD_CHAR(d)   (d + '0')

#ifdef MIN
#undef MIN
#undef MAX
#endif
#define MAX(a,b)      ((a)>(b)?(a):(b))
#define MIN(a,b)      ((a)>(b)?(b):(a))
#define ODD(a)        ((a)&1)

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif



/* prototypes */
_PROTOTYPE(void free_num, (bc_num *num));
_PROTOTYPE(bc_num new_num, (int length, int scale));
_PROTOTYPE(void init_numbers, (void));
_PROTOTYPE(bc_num copy_num, (bc_num num));
_PROTOTYPE(void init_num, (bc_num *num));
_PROTOTYPE(void str2num, (bc_num *num, char *str, int scale));
_PROTOTYPE(char *num2str, (bc_num num));
_PROTOTYPE(void int2num, (bc_num *num, int val));
_PROTOTYPE(long num2long, (bc_num num));
_PROTOTYPE(int bc_compare, (bc_num n1, bc_num n2));
_PROTOTYPE(char is_zero, (bc_num num));
_PROTOTYPE(char is_neg, (bc_num num));
_PROTOTYPE(void bc_add, (bc_num n1, bc_num n2, bc_num *result, int
scale_min));
_PROTOTYPE(void bc_sub, (bc_num n1, bc_num n2, bc_num *result, int
scale_min));
_PROTOTYPE(void bc_multiply, (bc_num n1, bc_num n2, bc_num *prod, int scale));
_PROTOTYPE(int bc_divide, (bc_num n1, bc_num n2, bc_num *quot, int scale));
_PROTOTYPE(int bc_modulo,
        (bc_num num1, bc_num num2, bc_num *result, int scale));
_PROTOTYPE(int bc_divmod,
        (bc_num num1, bc_num num2, bc_num *quot, bc_num *rem, int scale));
_PROTOTYPE(int bc_raisemod,
        (bc_num base, bc_num expo, bc_num mod, bc_num *result, int scale));
_PROTOTYPE(void bc_raise,
        (bc_num num1, bc_num num2, bc_num *result, int scale));
_PROTOTYPE(int bc_sqrt, (bc_num *num, int scale));
_PROTOTYPE(void out_long, (long val, int size, int space,
               void (*out_char)(int)));
_PROTOTYPE(void out_num, (bc_num num, int o_base, void (* out_char)(int)));


#endif  /* BIGPLUS_H */
