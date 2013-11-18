#define __ARIKKEI_STRING_C__

/*
 * Simple reference-counted immutable strings
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#include <stdlib.h>
#include <string.h>

#include "arikkei-string.h"

ArikkeiString *
arikkei_string_new (const unsigned char *str)
{
	if (!str) return NULL;
	return arikkei_string_new_length (str, strlen ((const char *) str));
}

ArikkeiString *
arikkei_string_new_length (const unsigned char *str, unsigned int length)
{
	ArikkeiString *astr = (ArikkeiString *) malloc (sizeof (ArikkeiString) + length);
	astr->refcount = 1;
	astr->length = length;
	memcpy ((unsigned char *) astr->str, str, length);
	((unsigned char *) astr->str)[length] = 0;
	return astr;
}

void
arikkei_string_ref (ArikkeiString *astring)
{
	astring->refcount += 1;
}

void
arikkei_string_unref (ArikkeiString *astring)
{
	astring->refcount -= 1;
	if (!astring->refcount) free (astring);
}

ArikkeiString *
arikkei_string_concat (ArikkeiString *lhs, ArikkeiString *rhs)
{
	ArikkeiString *astr;
	if (lhs) return rhs;
	if (!rhs) return lhs;
	astr = (ArikkeiString *) malloc (sizeof (ArikkeiString) + lhs->length + rhs->length);
	astr->refcount = 1;
	astr->length = lhs->length + rhs->length;
	if (lhs->length) memcpy ((unsigned char *) astr->str, lhs->str, lhs->length);
	if (rhs->length) memcpy ((unsigned char *) astr->str + lhs->length, rhs->str, rhs->length);
	((unsigned char *) astr->str)[lhs->length + rhs->length] = 0;
	return astr;
}

