#ifndef __ARIKKEI_ARRAY_H__
#define __ARIKKEI_ARRAY_H__

/*
 * Linear collection interface
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#define ARIKKEI_TYPE_ARRAY (arikkei_array_get_type ())
#define ARIKKEI_ARRAY(c) ((ArikkeiArray *) (c))

typedef struct _ArikkeiArray ArikkeiArray;
typedef struct _ArikkeiArrayClass ArikkeiArrayClass;

#include <libarikkei/arikkei-collection.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * An hybrid interface that allows sequential access by index
 * The iterator of an array is always unsigned integer
 */

struct _ArikkeiArray {
	ArikkeiCollection collection;
	void *(* get_element) (ArikkeiArray *array, void *array_instance, unsigned int index);
};

struct _ArikkeiArrayClass {
	ArikkeiCollectionClass collection_class;
};

unsigned int arikkei_array_get_type (void);

void *arikkei_array_get_element (ArikkeiArray *array, void *attay_instance, unsigned int index);

#ifdef __cplusplus
};
#endif

#endif

