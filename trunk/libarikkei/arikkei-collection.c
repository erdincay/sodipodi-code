#define __ARIKKEI_COLLECTION_C__

/*
 * Pure collection interface
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013-2014
 * 
 */

#include <stdlib.h>

#include "arikkei-collection.h"

unsigned int
arikkei_collection_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		arikkei_register_interface_type (&type, ARIKKEI_TYPE_INTERFACE, (const unsigned char *) "ArikkeiArray",
			sizeof (ArikkeiCollectionClass), sizeof (ArikkeiCollection), 0,
			NULL,
			NULL,
			NULL, NULL);
	}
	return type;
}

unsigned int
arikkei_collection_get_element_type (ArikkeiCollection *collection, void *collection_instance)
{
	if (collection->get_element_type) {
		return collection->get_element_type (collection, collection_instance);
	}
	return ARIKKEI_TYPE_NONE;
}

unsigned int
arikkei_collection_get_size (ArikkeiCollection *collection, void *collection_instance)
{
	if (collection->get_size) {
		return collection->get_size (collection, collection_instance);
	}
	return 0;
}

unsigned int
arikkei_collection_get_iterator (ArikkeiCollection *collection, void *collection_instance, ArikkeiValue *iterator)
{
	if (collection->get_iterator) {
		return collection->get_iterator (collection, collection_instance, iterator);
	}
	return 0;
}

unsigned int
arikkei_collection_iterator_next (ArikkeiCollection *collection, void *collection_instance, ArikkeiValue *iterator)
{
	if (collection->iterator_next) {
		return collection->iterator_next (collection, collection_instance, iterator);
	}
	return 0;
}

const void *
arikkei_collection_get_element (ArikkeiCollection *collection, void *collection_instance, ArikkeiValue *iterator)
{
	if (collection->get_element) {
		return collection->get_element (collection, collection_instance, iterator);
	}
	return NULL;
}


