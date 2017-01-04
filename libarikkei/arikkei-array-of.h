#ifndef __ARIKKEI_ARRAY_OF_H__
#define __ARIKKEI_ARRAY_OF_H__

/*
 * Value arrays
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 *
 */

typedef struct _AZArrayOf ArikkeiArrayOf;
typedef struct _AZArrayOfClass ArikkeiArrayOfClass;

#define ARIKKEI_TYPE_ARRAY_OF(t) az_array_of_get_type (t)

#include <az/array-of.h>

#ifdef __cplusplus
extern "C" {
#endif

#define arikkei_register_array_of_type az_register_array_of_type

#ifdef __cplusplus
};
#endif

#endif

