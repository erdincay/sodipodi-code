#ifndef __ARIKKEI_COLLECTION_H__
#define __ARIKKEI_COLLECTION_H__

/*
 * Pure collection interface
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#include <libarikkei/arikkei-types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _ArikkeiCollection {
	unsigned int is_static : 1;
	unsigned int can_read : 1;
	unsigned int can_write : 1;
	unsigned int is_final : 1;
	unsigned int (* get_element_type) (ArikkeiCollection *iface, void *instance);
	unsigned int (* get_length) (ArikkeiCollection *iface, void *instance);
	void (* get_element) (ArikkeiCollection *iface, void *instance, unsigned int idx, ArikkeiValue *val);
	void (* set_element) (ArikkeiCollection *iface, void *instance, unsigned int idx, const ArikkeiValue *val);
};

unsigned int arikkei_collection_get_element_type (ArikkeiCollection *iface, void *instance);
unsigned int arikkei_collection_get_length (ArikkeiCollection *iface, void *instance);
void arikkei_collection_get_element (ArikkeiCollection *iface, void *instance, unsigned int idx, ArikkeiValue *val);
void arikkei_collection_set_element (ArikkeiCollection *iface, void *instance, unsigned int idx, const ArikkeiValue *val);

#ifdef __cplusplus
};
#endif

#endif

