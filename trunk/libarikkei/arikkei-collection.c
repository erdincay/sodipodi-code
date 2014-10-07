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
		arikkei_register_interface_type (&type, ARIKKEI_TYPE_INTERFACE, (const unsigned char *) "ArikkeiCollection",
			sizeof (ArikkeiCollectionClass), sizeof (ArikkeiCollectionImplementation), 0,
			NULL,
			NULL,
			NULL, NULL);
	}
	return type;
}

unsigned int
arikkei_collection_get_element_type (ArikkeiCollectionImplementation *impl, void *collection_instance)
{
	if (impl->get_element_type) {
		return impl->get_element_type (impl, collection_instance);
	}
	return ARIKKEI_TYPE_NONE;
}

unsigned int
arikkei_collection_get_size (ArikkeiCollectionImplementation *impl, void *collection_instance)
{
	if (impl->get_size) {
		return impl->get_size (impl, collection_instance);
	}
	return 0;
}

unsigned int
arikkei_collection_get_iterator (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator)
{
	if (impl->get_iterator) {
		return impl->get_iterator (impl, collection_instance, iterator);
	}
	return 0;
}

unsigned int
arikkei_collection_iterator_next (ArikkeiCollectionImplementation *impl, void *collection_instance, ArikkeiValue *iterator)
{
	if (impl->iterator_next) {
		return impl->iterator_next (impl, collection_instance, iterator);
	}
	return 0;
}

unsigned int
arikkei_collection_get_element (ArikkeiCollectionImplementation *impl, void *collection_instance, const ArikkeiValue *iterator, ArikkeiValue *value)
{
	if (impl->get_element) {
		return impl->get_element (impl, collection_instance, iterator, value);
	}
	return 0;
}


