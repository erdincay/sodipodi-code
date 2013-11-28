#define __ARIKKEI_COLLECTION_C__

/*
 * Pure collection interface
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#include "arikkei-collection.h"

unsigned int
arikkei_collection_get_element_type (ArikkeiCollection *iface, void *instance)
{
	if (iface->get_element_type) {
		return iface->get_element_type (iface, instance);
	}
	return ARIKKEI_TYPE_NONE;
}

unsigned int
arikkei_collection_get_length (ArikkeiCollection *iface, void *instance)
{
	if (iface->get_length) {
		return iface->get_length (iface, instance);
	}
	return 0;
}

void
arikkei_collection_get_element (ArikkeiCollection *iface, void *instance, unsigned int idx, ArikkeiValue *val)
{
	if (!iface->can_read) return;
	if (iface->get_element) {
		iface->get_element (iface, instance, idx, val);
	}
}

void
arikkei_collection_set_element (ArikkeiCollection *iface, void *instance, unsigned int idx, const ArikkeiValue *val)
{
	if (!iface->can_write || iface->is_final) return;
	if (iface->set_element) {
		iface->set_element (iface, instance, idx, val);
	}
}

