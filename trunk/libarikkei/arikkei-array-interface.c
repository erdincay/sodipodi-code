#define __ARIKKEI_ARRAY_C__

/*
 * Linear collection interface
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#include <stdlib.h>

#include "arikkei-value.h"

#include "arikkei-array-interface.h"

static void arikkei_array_implementation_init (ArikkeiInterfaceImplementation *implementation);
static unsigned int arikkei_array_collection_get_iterator (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator);
static unsigned int arikkei_array_collection_iterator_next (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator);
static unsigned int arikkei_array_collection_get_element (ArikkeiCollectionImplementation *impl, void *collection_instance, const ArikkeiValue *iterator, ArikkeiValue *value);

unsigned int
arikkei_array_interface_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		ArikkeiInterfaceClass *ifklass;
		ifklass = arikkei_register_interface_type (&type, ARIKKEI_TYPE_COLLECTION, (const unsigned char *) "ArikkeiArrayInterface",
			sizeof (ArikkeiArrayInterfaceClass), sizeof (ArikkeiArrayImplementation), 0,
			NULL,
			arikkei_array_implementation_init,
			NULL, NULL);
		((ArikkeiClass *) ifklass)->zero_memory = 1;
	}
	return type;
}

static void
arikkei_array_implementation_init (ArikkeiInterfaceImplementation *implementation)
{
	ArikkeiCollectionImplementation *collection = (ArikkeiCollectionImplementation *) implementation;
	collection->get_iterator = arikkei_array_collection_get_iterator;
	collection->iterator_next = arikkei_array_collection_iterator_next;
	collection->get_element = arikkei_array_collection_get_element;
}

static unsigned int
arikkei_array_collection_get_iterator (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator)
{
	arikkei_value_set_u32 (iterator, 0);
	return 1;
}

static unsigned int
arikkei_array_collection_iterator_next (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator)
{
	if (iterator->uvalue >= arikkei_collection_get_size (impl, collection_instance)) return 0;
	iterator->uvalue += 1;
	return 1;
}

static unsigned int
arikkei_array_collection_get_element (ArikkeiCollectionImplementation *impl, void *collection_instance, const ArikkeiValue *iterator, ArikkeiValue *value)
{
	return arikkei_array_interface_get_element ((ArikkeiArrayImplementation *) impl, collection_instance, (unsigned int) iterator->ivalue, value);
}

unsigned int
arikkei_array_interface_get_element (ArikkeiArrayImplementation *impl, void *array_instance, unsigned int index, ArikkeiValue *value)
{
	if (impl->get_element) {
		return impl->get_element (impl, array_instance, index, value);
	}
	return 0;
}
