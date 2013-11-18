#ifndef __ARIKKEI_STRING_H__
#define __ARIKKEI_STRING_H__

/*
 * Simple reference-counted immutable strings
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#include <libarikkei/arikkei-types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _ArikkeiString {
	u32 refcount;
	u32 length;
	const unsigned char str[1];
};

ArikkeiString *arikkei_string_new (const unsigned char *str);
ArikkeiString *arikkei_string_new_length (const unsigned char *str, unsigned int length);

void arikkei_string_ref (ArikkeiString *astring);
void arikkei_string_unref (ArikkeiString *astring);

ArikkeiString *arikkei_string_concat (ArikkeiString *lhs, ArikkeiString *rhs);

#ifdef __cplusplus
};
#endif

#endif

