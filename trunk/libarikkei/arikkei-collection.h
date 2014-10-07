#ifndef __ARIKKEI_COLLECTION_H__
#define __ARIKKEI_COLLECTION_H__

/*
 * Pure collection interface
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013-2014
 * 
 */

#define ARIKKEI_TYPE_COLLECTION (arikkei_collection_get_type ())
#define ARIKKEI_COLLECTION(c) ((ArikkeiCollectionImplementation *) (c))

typedef struct _ArikkeiCollectionImplementation ArikkeiCollectionImplementation;
typedef struct _ArikkeiCollectionClass ArikkeiCollectionClass;

#include <libarikkei/arikkei-interface.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Normally you cannot make a collection of pure interfaces because you will need per-element implementation details
 */

struct _ArikkeiCollectionImplementation {
	ArikkeiInterfaceImplementation implementation;
	unsigned int (* get_element_type) (ArikkeiCollectionImplementation *impl, void *collection_instance);
	unsigned int (* get_size) (ArikkeiCollectionImplementation *impl, void *collection_instance);
	unsigned int (* get_iterator) (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator);
	unsigned int (* iterator_next) (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator);
	unsigned int (* get_element) (ArikkeiCollectionImplementation *impl, void *collection_instance, const ArikkeiValue *iterator, ArikkeiValue *value);
};

struct _ArikkeiCollectionClass {
	ArikkeiInterfaceClass interface_class;
};

unsigned int arikkei_collection_get_type (void);

/* Returns base type that all elements are guaranteed to be assignable to */
unsigned int arikkei_collection_get_element_type (ArikkeiCollectionImplementation *impl, void *collection_instance);
unsigned int arikkei_collection_get_size (ArikkeiCollectionImplementation *impl, void *collection_instance);
/* Return 0 if unusuccessful */
unsigned int arikkei_collection_get_iterator (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator);
unsigned int arikkei_collection_iterator_next (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator);
/* Value will contain the proper type of given element */
unsigned int arikkei_collection_get_element (ArikkeiCollectionImplementation *impl, void *collection_instance, const ArikkeiValue *iterator, ArikkeiValue *value);

#ifdef __cplusplus
};
#endif

#endif

