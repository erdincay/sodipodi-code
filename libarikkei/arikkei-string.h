#ifndef __ARIKKEI_STRING_H__
#define __ARIKKEI_STRING_H__

/*
 * Simple reference-counted immutable strings
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

typedef struct _ArikkeiClass ArikkeiStringClass;

#include <az/string.h>
#include <libarikkei/arikkei-types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define arikkei_string_new az_string_new
#define arikkei_string_new_length az_string_new_length

#define arikkei_string_ref az_string_ref
#define arikkei_string_unref az_string_unref

#define arikkei_string_concat az_string_concat

#ifdef __cplusplus
};
#endif

#endif

