#ifndef __ARIKKEI_COLLECTION_H__
#define __ARIKKEI_COLLECTION_H__

/*
 * Pure collection interface
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013-2014
 * 
 */

#define ARIKKEI_TYPE_COLLECTION (arikkei_collection_get_type ())
#define ARIKKEI_COLLECTION(c) ((ArikkeiCollection *) (c))

typedef struct _ArikkeiCollection ArikkeiCollection;
typedef struct _ArikkeiCollectionClass ArikkeiCollectionClass;

#include <libarikkei/arikkei-interface.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Normally you cannot make a collection of pure interfaces because you will need per-element implementation details
 */

struct _ArikkeiCollection {
	ArikkeiInterfaceImplementation implementation;
	unsigned int (* get_element_type) (ArikkeiCollection *collection, void *collection_instance);
	unsigned int (* get_size) (ArikkeiCollection *collection, void *collection_instance);
	unsigned int (* get_iterator) (ArikkeiCollection *collection, void *collection_instance, ArikkeiValue *iterator);
	unsigned int (* iterator_next) (ArikkeiCollection *collection, void *collection_instance, ArikkeiValue *iterator);
	const void *(* get_element) (ArikkeiCollection *collection, void *collection_instance, ArikkeiValue *iterator);
};

struct _ArikkeiCollectionClass {
	ArikkeiInterfaceClass interface_class;
};

unsigned int arikkei_collection_get_type (void);

unsigned int arikkei_collection_get_element_type (ArikkeiCollection *collection, void *collection_instance);
unsigned int arikkei_collection_get_size (ArikkeiCollection *collection, void *collection_instance);
unsigned int arikkei_collection_get_iterator (ArikkeiCollection *collection, void *collection_instance, ArikkeiValue *iterator);
unsigned int arikkei_collection_iterator_next (ArikkeiCollection *collection, void *collection_instance, ArikkeiValue *iterator);
const void *arikkei_collection_get_element (ArikkeiCollection *collection, void *collection_instance, ArikkeiValue *iterator);

#ifdef __cplusplus
};
#endif

#endif

