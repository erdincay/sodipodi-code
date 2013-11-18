#define __ARIKKEI_ACTIVE_OBJECT_C__

/*
 * Generic object system
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <malloc.h>
#include <string.h>

#ifdef WIN32
#define strdup _strdup
#endif

#include "arikkei-active-object.h"

/* ArikkeiActiveObject */

static void arikkei_active_object_class_init (ArikkeiActiveObjectClass *klass);
static void arikkei_active_object_finalize (ArikkeiActiveObject *aobj);

static ArikkeiObjectClass *parent_class;

unsigned int
arikkei_active_object_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		arikkei_register_type (&type, ARIKKEI_TYPE_OBJECT,
						(const unsigned char *) "ArikkeiActiveObject",
						sizeof (ArikkeiActiveObjectClass),
						sizeof (ArikkeiActiveObject),
						(void (*) (ArikkeiClass *)) arikkei_active_object_class_init,
						NULL,
						(void (*) (void *)) arikkei_active_object_finalize);
	}
	return type;
}

static void
arikkei_active_object_class_init (ArikkeiActiveObjectClass *klass)
{
	parent_class = (ArikkeiObjectClass *) ((ArikkeiClass *) klass)->parent;
}

static void
arikkei_active_object_finalize (ArikkeiActiveObject *aobj)
{
	if (aobj->attributes) {
		unsigned int i;
		for (i = 0; i < aobj->attributes->length; i++) {
			free (aobj->attributes->attribs[i].key);
			arikkei_value_clear (&aobj->attributes->attribs[i].value);
		}
		free (aobj->attributes);
	}
	if (aobj->callbacks) {
		unsigned int i;
		for (i = 0; i < aobj->callbacks->length; i++) {
			ArikkeiObjectListener *listener;
			listener = aobj->callbacks->listeners + i;
			if (listener->vector->dispose) listener->vector->dispose (aobj, listener->data);
		}
		free (aobj->callbacks);
	}
}

static ArikkeiObjectAttribute *
arikkei_active_object_get_attribute_slot (ArikkeiActiveObject *aobj, const unsigned char *key, unsigned int create)
{
	if (aobj->attributes) {
		unsigned int i;
		for (i = 0; i < aobj->attributes->length; i++) {
			if (!strcmp ((const char *) aobj->attributes->attribs[i].key, (const char *) key)) {
				return &aobj->attributes->attribs[i];
			}
		}
	}
	if (!create) return NULL;
	if (!aobj->attributes) {
		aobj->attributes = (ArikkeiObjectAttributeArray *) malloc (sizeof (ArikkeiObjectAttributeArray) + 3 * sizeof (ArikkeiObjectAttribute));
		memset (aobj->attributes, 0, sizeof (ArikkeiObjectAttributeArray) + 3 * sizeof (ArikkeiObjectAttribute));
		aobj->attributes->size = 4;
		aobj->attributes->length = 0;
	} else if (aobj->attributes->length >= aobj->attributes->size) {
		aobj->attributes->size <<= 1;
		aobj->attributes = (ArikkeiObjectAttributeArray *) realloc (aobj->attributes, sizeof (ArikkeiObjectAttributeArray) + (aobj->attributes->size - 1) * sizeof (ArikkeiObjectAttribute));
		memset (aobj->attributes, 0, sizeof (ArikkeiObjectAttributeArray) + (aobj->attributes->size - aobj->attributes->length) * sizeof (ArikkeiObjectAttribute));
	}
	aobj->attributes->attribs[aobj->attributes->length].key = (unsigned char *) strdup ((const char *) key);
	return &aobj->attributes->attribs[aobj->attributes->length++];
}

unsigned int
arikkei_active_object_get_attribute (ArikkeiActiveObject *aobj, const unsigned char *key, ArikkeiValue *val)
{
	ArikkeiObjectAttribute *attr;
	arikkei_return_val_if_fail (ARIKKEI_IS_ACTIVE_OBJECT (aobj), 0);
	arikkei_return_val_if_fail (key != NULL, 0);
	arikkei_return_val_if_fail (val != NULL, 0);
	attr = arikkei_active_object_get_attribute_slot (aobj, key, 0);
	if (attr) {
		arikkei_value_copy (val, &attr->value);
		return 1;
	}
	return 0;
}

unsigned int
arikkei_active_object_set_attribute (ArikkeiActiveObject *aobj, const unsigned char *key, const ArikkeiValue *val)
{
	ArikkeiObjectAttribute *attr;
	arikkei_return_val_if_fail (ARIKKEI_IS_ACTIVE_OBJECT (aobj), 0);
	arikkei_return_val_if_fail (key != NULL, 0);
	if (!val) return arikkei_active_object_clear_attribute (aobj, key);
	attr = arikkei_active_object_get_attribute_slot (aobj, key, 1);
	arikkei_value_copy (&attr->value, val);
	return 1;
}

unsigned int
arikkei_active_object_clear_attribute (ArikkeiActiveObject *aobj, const unsigned char *key)
{
	unsigned int i;
	arikkei_return_val_if_fail (ARIKKEI_IS_ACTIVE_OBJECT (aobj), 0);
	arikkei_return_val_if_fail (key != NULL, 0);
	if (!aobj->attributes) return 0;
	for (i = 0; i < aobj->attributes->length; i++) {
		if (!strcmp ((const char *) aobj->attributes->attribs[i].key, (const char *) key)) {
			arikkei_value_clear (&aobj->attributes->attribs[i].value);
			if ((i + 1) < aobj->attributes->length) {
				memcpy (&aobj->attributes->attribs[i], &aobj->attributes->attribs[i + 1], (aobj->attributes->length - (i + 1)) * sizeof (ArikkeiObjectAttribute));
			}
			aobj->attributes->length -= 1;
			if (!aobj->attributes->length) {
				free (aobj->attributes);
				aobj->attributes = NULL;
			}
			return 1;
		}
	}
	return 0;
}

void
arikkei_active_object_add_listener (ArikkeiActiveObject *aobj, const ArikkeiObjectEventVector *vector, unsigned int size, void *data)
{
	ArikkeiObjectListener *listener;

	if (!aobj->callbacks) {
		aobj->callbacks = (ArikkeiObjectCallbackBlock *) malloc (sizeof (ArikkeiObjectCallbackBlock));
		aobj->callbacks->size = 1;
		aobj->callbacks->length = 0;
	}
	if (aobj->callbacks->length >= aobj->callbacks->size) {
		int newsize;
		newsize = aobj->callbacks->size << 1;
		aobj->callbacks = (ArikkeiObjectCallbackBlock *) realloc (aobj->callbacks, sizeof (ArikkeiObjectCallbackBlock) + (newsize - 1) * sizeof (ArikkeiObjectListener));
		aobj->callbacks->size = newsize;
	}
	listener = aobj->callbacks->listeners + aobj->callbacks->length;
	listener->vector = vector;
	listener->size = size;
	listener->data = data;
	aobj->callbacks->length += 1;
}

void
arikkei_active_object_remove_listener_by_data (ArikkeiActiveObject *aobj, void *data)
{
	if (aobj->callbacks) {
		unsigned int i;
		for (i = 0; i < aobj->callbacks->length; i++) {
			ArikkeiObjectListener *listener;
			listener = aobj->callbacks->listeners + i;
			if (listener->data == data) {
				aobj->callbacks->length -= 1;
				if (aobj->callbacks->length < 1) {
					free (aobj->callbacks);
					aobj->callbacks = NULL;
				} else if (aobj->callbacks->length != i) {
					*listener = aobj->callbacks->listeners[aobj->callbacks->length];
				}
				return;
			}
		}
	}
}

unsigned int
arikkei_active_object_set_attribute_i32 (ArikkeiActiveObject *aobj, const unsigned char *key, int value)
{
	unsigned int result;
	ArikkeiValue val = { 0 };
	arikkei_value_set_i32 (&val, value);
	result = arikkei_active_object_set_attribute (aobj, key, &val);
	arikkei_value_clear (&val);
	return result;
}

unsigned int
arikkei_active_object_set_attribute_object (ArikkeiActiveObject *aobj, const unsigned char *key, ArikkeiObject *value)
{
	unsigned int result;
	ArikkeiValue val = { 0 };
	arikkei_value_set_object (&val, value);
	result = arikkei_active_object_set_attribute (aobj, key, &val);
	arikkei_value_clear (&val);
	return result;
}


