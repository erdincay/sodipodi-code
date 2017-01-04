#ifndef __ARIKKEI_VALUE_ARRAY_H__
#define __ARIKKEI_VALUE_ARRAY_H__

/*
 * Generic object system
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#define ARIKKEI_TYPE_VALUE_ARRAY az_value_array_get_type ()
#define ARIKKEI_VALUE_ARRAY(o) ((ArikkeiValueArray *) (o))

typedef struct _AZValueArray ArikkeiValueArray;
typedef struct _AZValueArrayClass ArikkeiValueArrayClass;

#include <az/value-array.h>

#ifdef __cplusplus
extern "C" {
#endif

#define arikkei_value_array_new az_value_array_new

#define arikkei_value_array_set_element az_value_array_set_element

#ifdef __cplusplus
};
#endif

#endif

