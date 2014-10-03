#define __ARIKKEI_OBJECT_C__

/*
 * RGBA display list system for sodipodi
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#include "arikkei-function.h"

#ifdef WIN32
#define strdup _strdup
#endif

#include "arikkei-object.h"

/* ArikkeiObject */

void *
arikkei_object_check_instance_cast (void *ip, unsigned int tc)
{
	arikkei_return_val_if_fail (ip != NULL, NULL);
	arikkei_return_val_if_fail (arikkei_type_is_a (((ArikkeiObject *) ip)->klass->klass.type, tc), NULL);
	return ip;
}

unsigned int
arikkei_object_check_instance_type (void *ip, unsigned int tc)
{
	if (ip == NULL) return 0;
	return arikkei_type_is_a (((ArikkeiObject *) ip)->klass->klass.type, tc);
}

void *
arikkei_object_check_interface_cast (void *ip, unsigned int tc)
{
	arikkei_return_val_if_fail (ip != NULL, NULL);
	arikkei_return_val_if_fail (arikkei_type_implements_a (((ArikkeiObject *) ip)->klass->klass.type, tc), NULL);
	return ip;
}

unsigned int
arikkei_object_check_interface_type (void *ip, unsigned int tc)
{
	if (ip == NULL) return 0;
	return arikkei_type_implements_a (((ArikkeiObject *) ip)->klass->klass.type, tc);
}

#if 0
void *
arikkei_object_check_interface_cast (void *ip, unsigned int tc)
{
	arikkei_return_val_if_fail (ip != NULL, NULL);
	arikkei_return_val_if_fail (arikkei_type_is_a (((ArikkeiObjectInterface *) ip)->implementation->implementation.klass->klass.type, tc), NULL);
	return ip;
}

unsigned int
arikkei_object_check_interface_type (void *ip, unsigned int tc)
{
	if (ip == NULL) return 0;
	return arikkei_type_is_a (((ArikkeiObjectInterface *) ip)->implementation->implementation.klass->klass.type, tc);
}
#endif

unsigned int
arikkei_object_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		ArikkeiClass *klass;
		klass = arikkei_register_type (&type, ARIKKEI_TYPE_STRUCT, (const unsigned char *) "ArikkeiObject", sizeof (ArikkeiObjectClass), sizeof (ArikkeiObject), NULL, NULL, NULL);
		klass->zero_memory = 1;
	}
	return type;
}

static void
arikkei_object_interface_instance_init (ArikkeiObjectImplementation *implementation,  ArikkeiObjectInstance *instance)
{
	instance->instance_offset = implementation->implementation.instance_offset;
	instance->class_offset = implementation->implementation.class_offset;
}

unsigned int
arikkei_object_interface_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		arikkei_register_interface_type (&type, ARIKKEI_TYPE_INTERFACE, (const unsigned char *) "ArikkeiObjectInterface",
			sizeof (ArikkeiObjectInterfaceClass), sizeof (ArikkeiObjectImplementation), sizeof (ArikkeiObjectInstance),
			NULL, NULL, (void (*) (ArikkeiInterfaceImplementation *, void *)) arikkei_object_interface_instance_init, NULL);
	}
	return type;
}

/* Dynamic lifecycle */

ArikkeiObject *
arikkei_object_new (unsigned int type)
{
	ArikkeiObjectClass *klass;
	ArikkeiObject *object;

	arikkei_return_val_if_fail (arikkei_type_is_a (type, ARIKKEI_TYPE_OBJECT), NULL);

	klass = (ArikkeiObjectClass *) arikkei_type_get_class (type);
	object = (ArikkeiObject *) malloc (klass->klass.instance_size);
	arikkei_object_setup (object, type);

	return object;
}

ArikkeiObject *
arikkei_object_delete (ArikkeiObject *object)
{
	arikkei_object_release (object);
	free (object);
	return NULL;
}

void
arikkei_object_ref (ArikkeiObject *object)
{
	object->refcount += 1;
}

void
arikkei_object_unref (ArikkeiObject *object)
{
	object->refcount -= 1;
	if (object->refcount < 1) {
		arikkei_object_delete (object);
	}
}

void
arikkei_object_instance_ref (ArikkeiObjectInstance *iface)
{
	ArikkeiObject *object = arikkei_object_instance_get_object (iface);
	arikkei_object_ref (object);
}

void
arikkei_object_instance_unref (ArikkeiObjectInstance *iface)
{
	ArikkeiObject *object = arikkei_object_instance_get_object (iface);
	arikkei_object_ref (object);
}

/* Automatic lifecycle */

void
arikkei_object_setup (ArikkeiObject *object, unsigned int type)
{
	ArikkeiClass *klass;
	arikkei_return_if_fail (arikkei_type_is_a (type, ARIKKEI_TYPE_OBJECT));
	klass = arikkei_type_get_class (type);
	arikkei_instance_setup (object, type);
	object->klass = (ArikkeiObjectClass *) klass;
	object->refcount = 1;
}

#if 0
void
arikkei_object_setup_interface (ArikkeiObject *object, ArikkeiObject *owner, unsigned int type)
{
	ArikkeiClass *klass;
	arikkei_return_if_fail (arikkei_type_is_a (type, ARIKKEI_TYPE_OBJECT));
	klass = arikkei_type_get_class (type);
	arikkei_instance_setup (object, type);
	object->klass = (ArikkeiObjectClass *) klass;
	object->is_interface = 1;
	if (owner) object->refcount = (const char *) object - (const char *) owner;
}
#endif

void
arikkei_object_release (ArikkeiObject *object)
{
	arikkei_return_if_fail (ARIKKEI_IS_OBJECT (object));
	arikkei_instance_release (object, object->klass->klass.type);
}

#if 0
void *
arikkei_object_get_interface (ArikkeiObject *object, unsigned int type)
{
	arikkei_return_val_if_fail (ARIKKEI_IS_OBJECT (object), NULL);
	return arikkei_instance_get_interface ((ArikkeiClass *) object->klass, object, type);
}

void *
arikkei_object_interface_get_owner (ArikkeiObject *object)
{
	arikkei_return_val_if_fail (ARIKKEI_IS_OBJECT (object), NULL);
	arikkei_return_val_if_fail (object->is_interface, NULL);
	return (ArikkeiObject *) ((const char *) object - object->refcount);
}
#endif

