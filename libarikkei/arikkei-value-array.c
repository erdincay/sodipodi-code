#define __ARIKKEI_VALUE_ARRAY_C__

/*
 * Generic object system
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <stdlib.h>

#include "arikkei-utils.h"
#include "arikkei-value.h"
#include "arikkei-value-array.h"

static void arikkei_value_array_class_init (ArikkeiValueArrayClass *klass);
static void arikkei_value_array_finalize (ArikkeiValueArray *varray);

/* ArikkeiClass implementation */
static unsigned int arikkei_value_array_get_property (ArikkeiClass *klass, void *instance, unsigned int idx, ArikkeiValue *value);
/* ArikkeiCollection implementation */
static unsigned int arikkei_value_array_get_element_type (ArikkeiCollectionImplementation *impl, void *collection_instance);
static unsigned int arikkei_value_array_get_size (ArikkeiCollectionImplementation *impl, void *collection_instance);
/* ArikkeiArray implementation */
static unsigned int arikkei_value_array_get_element (ArikkeiArrayImplementation *impl, void *array_instance, unsigned int index, ArikkeiValue *value);

enum {
	PROP_LENGTH,
	NUM_PROPERTIES
};

unsigned int
arikkei_value_array_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		ArikkeiClass *klass;
		klass = arikkei_register_type (&type, ARIKKEI_TYPE_REFERENCE, (const unsigned char *) "ArikkeiValueArray",
			sizeof (ArikkeiValueArrayClass), sizeof (ArikkeiValueArray),
			(void (*) (ArikkeiClass *)) arikkei_value_array_class_init,
			NULL,
			(void (*) (void *)) arikkei_value_array_finalize);
		klass->zero_memory = 1;
	}
	return type;
}

static void
arikkei_value_array_class_init (ArikkeiValueArrayClass *klass)
{
	arikkei_class_set_num_interfaces ((ArikkeiClass *) klass, 1);
	arikkei_interface_implementation_setup ((ArikkeiClass *) klass, 0, ARIKKEI_TYPE_ARRAY_INTERFACE, ARIKKEI_OFFSET(ArikkeiValueArrayClass, array_implementation), 0);
	arikkei_class_set_num_properties ((ArikkeiClass *) klass, NUM_PROPERTIES);
	arikkei_class_property_setup ((ArikkeiClass *) klass, PROP_LENGTH, (const unsigned char *) "length", ARIKKEI_TYPE_UINT32, 0, 1, 0, 1, 0, ARIKKEI_TYPE_NONE, NULL);
	((ArikkeiClass *) klass)->get_property = arikkei_value_array_get_property;
	klass->array_implementation.collection_implementation.get_element_type = arikkei_value_array_get_element_type;
	klass->array_implementation.collection_implementation.get_size = arikkei_value_array_get_size;
	klass->array_implementation.get_element = arikkei_value_array_get_element;
}

static void
arikkei_value_array_finalize (ArikkeiValueArray *varray)
{
	unsigned int i;
	for (i = 0; i < varray->length; i++) {
		arikkei_value_clear (&varray->values[i]);
	}
	if (varray->values) free (varray->values);
}

static unsigned int
arikkei_value_array_get_property (ArikkeiClass *klass, void *instance, unsigned int idx, ArikkeiValue *value)
{
	ArikkeiValueArray *varray = ARIKKEI_VALUE_ARRAY(instance);
	switch (idx) {
	case PROP_LENGTH:
		arikkei_value_set_u32 (value, varray->length);
		return 1;
	default:
		break;
	}
	return 0;
}

static unsigned int
arikkei_value_array_get_element_type (ArikkeiCollectionImplementation *impl, void *collection_instance)
{
	return ARIKKEI_TYPE_ANY;
}

static unsigned int
arikkei_value_array_get_size (ArikkeiCollectionImplementation *impl, void *collection_instance)
{
	ArikkeiValueArray *varray = (ArikkeiValueArray *) arikkei_interface_get_containing_instance ((ArikkeiInterfaceImplementation *) impl, collection_instance);
	return varray->length;
}

static unsigned int
arikkei_value_array_get_element (ArikkeiArrayImplementation *impl, void *array_instance, unsigned int index, ArikkeiValue *value)
{
	ArikkeiValueArray *varray;
	arikkei_return_val_if_fail (impl != NULL, 0);
	arikkei_return_val_if_fail (array_instance != NULL, 0);
	arikkei_return_val_if_fail (value != NULL, 0);
	varray = (ArikkeiValueArray *) arikkei_interface_get_containing_instance ((ArikkeiInterfaceImplementation *) impl, array_instance);
	arikkei_return_val_if_fail (index < varray->length, 0);
	arikkei_value_copy (value, &varray->values[index]);
	return 1;
}

ArikkeiValueArray *
arikkei_value_array_new (unsigned int length)
{
	ArikkeiValueArray *varray = (ArikkeiValueArray *) arikkei_instance_new (ARIKKEI_TYPE_VALUE_ARRAY);
	varray->length = length;
	varray->values = (ArikkeiValue *) arikkei_instance_new_array (ARIKKEI_TYPE_VALUE, varray->length);
	return varray;
}

void
arikkei_value_array_set_element (ArikkeiValueArray *varray, unsigned int idx, const ArikkeiValue *value)
{
	arikkei_return_if_fail (varray != NULL);
	arikkei_return_if_fail (idx < varray->length);
	arikkei_return_if_fail (value != NULL);
	arikkei_value_copy (&varray->values[idx], value);
}
