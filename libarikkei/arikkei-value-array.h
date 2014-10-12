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

#define ARIKKEI_TYPE_VALUE_ARRAY (arikkei_value_array_get_type ())
#define ARIKKEI_VALUE_ARRAY(o) ((ArikkeiValueArray *) (o))

typedef struct _ArikkeiValueArray ArikkeiValueArray;
typedef struct _ArikkeiValueArrayClass ArikkeiValueArrayClass;

#include <libarikkei/arikkei-array.h>
#include <libarikkei/arikkei-reference.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _ArikkeiValueArray {
	ArikkeiReference reference;
	unsigned int length;
	ArikkeiValue *values;
};

struct _ArikkeiValueArrayClass {
	ArikkeiReferenceClass reference_klass;
	ArikkeiArrayImplementation array_implementation;
};

unsigned int arikkei_value_array_get_type (void);

ArikkeiValueArray *arikkei_value_array_new (unsigned int length);

void arikkei_value_array_set_element (ArikkeiValueArray *varray, unsigned int idx, const ArikkeiValue *value);

#ifdef __cplusplus
};
#endif

#endif

