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

unsigned int
arikkei_object_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		arikkei_register_type (&type, ARIKKEI_TYPE_STRUCT, (const unsigned char *) "ArikkeiObject", sizeof (ArikkeiObjectClass), sizeof (ArikkeiObject), NULL, NULL, NULL);
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
	if (!object->is_interface) {
		object->refcount += 1;
	} else if (object->refcount != 0) {
		object = (ArikkeiObject *) ((const char *) object - object->refcount);
		arikkei_object_ref (object);
	}
}

void
arikkei_object_unref (ArikkeiObject *object)
{
	if (!object->is_interface) {
		object->refcount -= 1;
		if (object->refcount < 1) {
			arikkei_object_delete (object);
		}
	} else if (object->refcount != 0) {
		object = (ArikkeiObject *) ((const char *) object - object->refcount);
		arikkei_object_unref (object);
	}
}

/* Automatic lifecycle */

void
arikkei_object_setup (ArikkeiObject *object, unsigned int type)
{
	ArikkeiClass *klass;
	arikkei_return_if_fail (arikkei_type_is_a (type, ARIKKEI_TYPE_OBJECT));
	klass = arikkei_type_get_class (type);
	arikkei_type_setup_instance (object, type);
	object->klass = (ArikkeiObjectClass *) klass;
	object->refcount = 1;
}

void
arikkei_object_setup_interface (ArikkeiObject *object, ArikkeiObject *owner, unsigned int type)
{
	ArikkeiClass *klass;
	arikkei_return_if_fail (arikkei_type_is_a (type, ARIKKEI_TYPE_OBJECT));
	klass = arikkei_type_get_class (type);
	arikkei_type_setup_instance (object, type);
	object->klass = (ArikkeiObjectClass *) klass;
	object->is_interface = 1;
	if (owner) object->refcount = (const char *) object - (const char *) owner;
}

void
arikkei_object_release (ArikkeiObject *object)
{
	arikkei_return_if_fail (ARIKKEI_IS_OBJECT (object));
	arikkei_type_release_instance (object, object->klass->klass.type);
}

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

void
arikkei_object_class_property_setup (ArikkeiObjectClass *klass, unsigned int idx, const unsigned char *key, unsigned int type,
									 unsigned int isstatic, unsigned int canread, unsigned int canwrite, unsigned int isfinal, void *value)
{
	arikkei_property_setup (((ArikkeiClass *) klass)->properties + idx, key, type, ((ArikkeiClass *) klass)->firstproperty + idx, isstatic, canread, canwrite, isfinal, value);
}

void
arikkei_object_class_method_setup (ArikkeiObjectClass *klass, unsigned int idx, const unsigned char *key,
									unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
									unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *))
{
	ArikkeiFunction *func;
	func = arikkei_function_new (klass->klass.type, rettype, nargs, argtypes, call);
	arikkei_object_class_property_setup (klass, idx, key, ARIKKEI_TYPE_FUNCTION, 0, 1, 0, 1, func);
	arikkei_object_unref ((ArikkeiObject *) func);
}

void
arikkei_object_class_static_method_setup (ArikkeiObjectClass *klass, unsigned int idx, const unsigned char *key,
										   unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
										   unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *))
{
	ArikkeiFunction *func;
	func = arikkei_function_new (ARIKKEI_TYPE_NONE, rettype, nargs, argtypes, call);
	arikkei_object_class_property_setup (klass, idx, key, ARIKKEI_TYPE_FUNCTION, 1, 1, 0, 1, func);
	arikkei_object_unref ((ArikkeiObject *) func);
}
