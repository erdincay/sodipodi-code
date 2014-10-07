#define __ARIKKEI_ARRAY_C__

/*
 * Linear collection interface
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#include <stdlib.h>

#include "arikkei-value.h"

#include "arikkei-array.h"

static void arikkei_array_implementation_init (ArikkeiInterfaceImplementation *implementation);
static unsigned int arikkei_array_collection_get_iterator (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator);
unsigned int arikkei_array_collection_get_element (ArikkeiCollectionImplementation *impl, void *collection_instance, const ArikkeiValue *iterator, ArikkeiValue *value);

unsigned int
arikkei_array_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		ArikkeiInterfaceClass *ifklass;
		ifklass = arikkei_register_interface_type (&type, ARIKKEI_TYPE_COLLECTION, (const unsigned char *) "ArikkeiArray",
			sizeof (ArikkeiInterfaceClass), sizeof (ArikkeiArrayImplementation), 0,
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
	collection->get_element = arikkei_array_collection_get_element;
}

static unsigned int
arikkei_array_collection_get_iterator (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator)
{
	arikkei_value_set_u32 (iterator, 0);
	return 1;
}

unsigned int
arikkei_array_collection_get_element (ArikkeiCollectionImplementation *impl, void *collection_instance, const ArikkeiValue *iterator, ArikkeiValue *value)
{
	return arikkei_array_get_element (ARIKKEI_ARRAY(impl), collection_instance, (unsigned int) iterator->ivalue, value);
}

unsigned int
arikkei_array_get_element (ArikkeiArrayImplementation *impl, void *array_instance, unsigned int index, ArikkeiValue *value)
{
	if (impl->get_element) {
		return impl->get_element (impl, array_instance, index, value);
	}
	return 0;
}
