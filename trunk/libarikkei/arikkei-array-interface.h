#ifndef __ARIKKEI_ARRAY_INTERFACE_H__
#define __ARIKKEI_ARRAY_INTERFACE_H__

/*
 * Linear collection interface
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#define ARIKKEI_TYPE_ARRAY_INTERFACE (arikkei_array_interface_get_type ())

typedef struct _ArikkeiArrayImplementation ArikkeiArrayImplementation;
typedef struct _ArikkeiArrayInterfaceClass ArikkeiArrayInterfaceClass;

#include <libarikkei/arikkei-collection.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * An hybrid interface that allows sequential access by index
 * The iterator of an array is always unsigned integer
 */

struct _ArikkeiArrayImplementation {
	ArikkeiCollectionImplementation collection_implementation;
	unsigned int (*get_element) (ArikkeiArrayImplementation *impl, void *array_instance, unsigned int index, ArikkeiValue *value);
};

struct _ArikkeiArrayInterfaceClass {
	ArikkeiCollectionClass collection_class;
};

unsigned int arikkei_array_interface_get_type (void);

unsigned int arikkei_array_interface_get_element (ArikkeiArrayImplementation *impl, void *array_instance, unsigned int index, ArikkeiValue *value);

#ifdef __cplusplus
};
#endif

#endif

