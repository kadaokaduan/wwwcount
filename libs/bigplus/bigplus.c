/* number.c: Implements arbitrary precision numbers. */

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

#ifdef __VMS
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "bigplus.h"

_PROTOTYPE(static bc_num _do_add, (bc_num n1, bc_num n2, int scale_min));
_PROTOTYPE(static void _rm_leading_zeros, (bc_num num));
_PROTOTYPE(static void out_of_memory, (void));




/* Storage used for special numbers. */
bc_num _zero_;
bc_num _one_;
bc_num _two_;



/* For many things, we may have leading zeros in a number NUM.
   _rm_leading_zeros just moves the data to the correct
   place and adjusts the length. */

static void
_rm_leading_zeros (num)
     bc_num num;
{
  int bytes;
  char *dst, *src;

  /* Do a quick check to see if we need to do it. */
  if (*num->n_value != 0) return;

  /* The first "digit" is 0, find the first non-zero digit in the second
     or greater "digit" to the left of the decimal place. */
  bytes = num->n_len;
  src = num->n_value;
  while (bytes > 1 && *src == 0) src++, bytes--;
  num->n_len = bytes;
  bytes += num->n_scale;
  dst = num->n_value;
  while (bytes-- > 0) *dst++ = *src++;

}

bc_num
copy_num (num)
     bc_num num;
{
  num->n_refs++;
  return num;
}
static void out_of_memory()
{
    (void) fprintf(stderr," xxx\n");
}

/* "Frees" a bc_num NUM.  Actually decreases reference count and only
   frees the storage if reference count is zero. */


void
free_num (num)
    bc_num *num;
{
  if (*num == NULL) return;
  (*num)->n_refs--;
  if ((*num)->n_refs == 0) free(*num);
  *num = NULL;
}



bc_num
new_num (length, scale)
     int length, scale;
{
  bc_num temp;

  temp = (bc_num) malloc (sizeof(bc_struct)+length+scale);
  if (temp == NULL) out_of_memory ();
  temp->n_sign = PLUS;
  temp->n_len = length;
  temp->n_scale = scale;
  temp->n_refs = 1;
  temp->n_value[0] = 0;
  return temp;
}

/* Intitialize the number package! */

void
init_numbers ()
{
  _zero_ = new_num (1,0);
  _one_  = new_num (1,0);
  _one_->n_value[0] = 1;
  _two_  = new_num (1,0);
  _two_->n_value[0] = 2;
}

/* Initialize a number NUM by making it a copy of zero. */

void
init_num (num)
     bc_num *num;
{
  *num = copy_num (_zero_);
}



static bc_num
_do_add (n1, n2, scale_min)
     bc_num n1, n2;
     int scale_min;
{
  bc_num sum;
  int sum_scale, sum_digits;
  char *n1ptr, *n2ptr, *sumptr;
  int carry, n1bytes, n2bytes;
  int count;

  /* Prepare sum. */
  sum_scale = MAX (n1->n_scale, n2->n_scale);
  sum_digits = MAX (n1->n_len, n2->n_len) + 1;
  sum = new_num (sum_digits, MAX(sum_scale, scale_min));

  /* Zero extra digits made by scale_min. */
  if (scale_min > sum_scale)
    {
      sumptr = (char *) (sum->n_value + sum_scale + sum_digits);
      for (count = scale_min - sum_scale; count > 0; count--)
	*sumptr++ = 0;
    }

  /* Start with the fraction part.  Initialize the pointers. */
  n1bytes = n1->n_scale;
  n2bytes = n2->n_scale;
  n1ptr = (char *) (n1->n_value + n1->n_len + n1bytes - 1);
  n2ptr = (char *) (n2->n_value + n2->n_len + n2bytes - 1);
  sumptr = (char *) (sum->n_value + sum_scale + sum_digits - 1);

  /* Add the fraction part.  First copy the longer fraction.*/
  if (n1bytes != n2bytes)
    {
      if (n1bytes > n2bytes)
	while (n1bytes>n2bytes)
	  { *sumptr-- = *n1ptr--; n1bytes--;}
      else
	while (n2bytes>n1bytes)
	  { *sumptr-- = *n2ptr--; n2bytes--;}
    }

  /* Now add the remaining fraction part and equal size integer parts. */
  n1bytes += n1->n_len;
  n2bytes += n2->n_len;
  carry = 0;
  while ((n1bytes > 0) && (n2bytes > 0))
    {
      *sumptr = *n1ptr-- + *n2ptr-- + carry;
      if (*sumptr > (BASE-1))
	{
	   carry = 1;
	   *sumptr -= BASE;
	}
      else
	carry = 0;
      sumptr--;
      n1bytes--;
      n2bytes--;
    }

  /* Now add carry the longer integer part. */
  if (n1bytes == 0)
    { n1bytes = n2bytes; n1ptr = n2ptr; }
  while (n1bytes-- > 0)
    {
      *sumptr = *n1ptr-- + carry;
      if (*sumptr > (BASE-1))
	{
	   carry = 1;
	   *sumptr -= BASE;
	 }
      else
	carry = 0;
      sumptr--;
    }

  /* Set final carry. */
  if (carry == 1)
    *sumptr += 1;

  /* Adjust sum and return. */
  _rm_leading_zeros (sum);
  return sum;
}


void bc_add (n1, n2, result, scale_min)
     bc_num n1, n2, *result;
     int scale_min;
{
  bc_num sum;

      sum = _do_add (n1, n2, scale_min);
      sum->n_sign = n1->n_sign;

  /* Clean up and return. */
  free_num (result);
  *result = sum;
}

char
*num2str (num)
      bc_num num;
{
  char *str, *sptr;
  char *nptr;
  int  index, signch;

  /* Allocate the string memory. */
  signch = ( num->n_sign == PLUS ? 0 : 1 );  /* Number of sign chars. */
  if (num->n_scale > 0)
    str = (char *) malloc (num->n_len + num->n_scale + 2 + signch);
  else
    str = (char *) malloc (num->n_len + 1 + signch);
  if (str == NULL) out_of_memory();

  /* The negative sign if needed. */
  sptr = str;
  if (signch) *sptr++ = '-';

  /* Load the whole number. */
  nptr = num->n_value;
  for (index=num->n_len; index>0; index--)
    *sptr++ = BCD_CHAR(*nptr++);

  /* Now the fraction. */
  if (num->n_scale > 0)
    {
      *sptr++ = '.';
      for (index=0; index<num->n_scale; index++)
	*sptr++ = BCD_CHAR(*nptr++);
    }

  /* Terminate the string and return it! */
  *sptr = '\0';
  return (str);
}


void
str2num (num, str, scale)
     bc_num *num;
     char *str;
     int scale;
{
  int digits, strscale;
  char *ptr, *nptr;
  char zero_int;

  /* Prepare num. */
  free_num (num);

  /* Check for valid number and count digits. */
  ptr = str;
  digits = 0;
  strscale = 0;
  zero_int = FALSE;
  if ( (*ptr == '+') || (*ptr == '-'))  ptr++;  /* Sign */
  while (*ptr == '0') ptr++;			/* Skip leading zeros. */
  while (isdigit(*ptr)) ptr++, digits++;	/* digits */
  if (*ptr == '.') ptr++;			/* decimal point */
  while (isdigit(*ptr)) ptr++, strscale++;	/* digits */
  if ((*ptr != '\0') || (digits+strscale == 0))
    {
      *num = copy_num (_zero_);
      return;
    }

  /* Adjust numbers and allocate storage and initialize fields. */
  strscale = MIN(strscale, scale);
  if (digits == 0)
    {
      zero_int = TRUE;
      digits = 1;
    }
  *num = new_num (digits, strscale);

  /* Build the whole number. */
  ptr = str;
  if (*ptr == '-')
    {
      (*num)->n_sign = MINUS;
      ptr++;
    }
  else
    {
      (*num)->n_sign = PLUS;
      if (*ptr == '+') ptr++;
    }
  while (*ptr == '0') ptr++;			/* Skip leading zeros. */
  nptr = (*num)->n_value;
  if (zero_int)
    {
      *nptr++ = 0;
      digits = 0;
    }
  for (;digits > 0; digits--)
    *nptr++ = CH_VAL(*ptr++);


  /* Build the fractional part. */
  if (strscale > 0)
    {
      ptr++;  /* skip the decimal point! */
      for (;strscale > 0; strscale--)
	*nptr++ = CH_VAL(*ptr++);
    }
}
