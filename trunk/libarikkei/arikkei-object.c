#define __ARIKKEI_OBJECT_C__

/*
 * Basic type and object system
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <malloc.h>
#include <string.h>
#include <stdio.h>

#if 0
#include "arikkei-function.h"
#endif

#include "arikkei-object.h"

/* ArikkeiObject */

static void arikkei_object_class_init (ArikkeiObjectClass *klass);
static void arikkei_object_init (ArikkeiObject *object);

unsigned int
arikkei_object_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		arikkei_register_type (&type, ARIKKEI_TYPE_STRUCT, (const unsigned char *) "ArikkeiObject",
			sizeof (ArikkeiObjectClass), sizeof (ArikkeiObject),
			(void (*) (ArikkeiClass *klass)) arikkei_object_class_init,
			(void (*) (void *)) arikkei_object_init,
			NULL);
	}
	return type;
}

static void
arikkei_object_class_init (ArikkeiObjectClass *klass)
{
	((ArikkeiClass *) klass)->zero_memory = 1;
}

static void
arikkei_object_init (ArikkeiObject *object)
{
	object->refcount = 1;
	object->flags |= ARIKKEI_OBJECT_ALIVE;
}

void
arikkei_object_ref (ArikkeiObject *object)
{
	object->refcount += 1;
}

void
arikkei_object_unref (ArikkeiObject *object)
{
	arikkei_return_if_fail (object->refcount > 0);
	if (object->refcount == 1) {
		if (!object->klass->drop || object->klass->drop (object)) {
			if (object->flags & ARIKKEI_OBJECT_ALIVE) {
				if (object->klass->dispose) {
					object->klass->dispose (object);
				}
			}
			arikkei_instance_delete (object->klass->klass.implementation.type, object);
		}
	} else {
		object->refcount -= 1;
	}
}

ArikkeiObject *
arikkei_object_new (unsigned int type)
{
	ArikkeiObject *object;
	arikkei_return_val_if_fail (arikkei_type_is_a (type, ARIKKEI_TYPE_OBJECT), NULL);
	object = (ArikkeiObject *) arikkei_instance_new (type);
	object->klass = (ArikkeiObjectClass *) arikkei_type_get_class (type);
	return object;
}

void
arikkei_object_dispose (ArikkeiObject *object)
{
	arikkei_return_if_fail (object != NULL);
	arikkei_return_if_fail (ARIKKEI_IS_OBJECT (object));
	arikkei_return_if_fail (object->flags & ARIKKEI_OBJECT_ALIVE);
	if (object->klass->dispose) {
		object->klass->dispose (object);
	}
	object->flags &= ~ARIKKEI_OBJECT_ALIVE;
	arikkei_object_unref (object);
}

void *
arikkei_object_check_instance_cast (void *ip, unsigned int tc)
{
	if (ip == NULL) {
		fprintf (stderr, "arikkei_object_check_instance_cast: ip == NULL\n");
		return NULL;
	}
	if (!arikkei_type_is_a (((ArikkeiObject *) ip)->klass->klass.implementation.type, tc)) {
		ArikkeiClass *klass = arikkei_type_get_class (tc);
		fprintf (stderr, "arikkei_object_check_instance_cast: %s is not %s\n", ((ArikkeiObject *) ip)->klass->klass.name, klass->name);
		return NULL;
	}
	return ip;
}

unsigned int
arikkei_object_check_instance_type (void *ip, unsigned int tc)
{
	if (ip == NULL) return 0;
	return arikkei_type_is_a (((ArikkeiObject *) ip)->klass->klass.implementation.type, tc);
}

unsigned int
arikkei_object_implements (ArikkeiObject *object, unsigned int type)
{
	arikkei_return_val_if_fail (object != NULL, 0);
	arikkei_return_val_if_fail (ARIKKEI_IS_OBJECT (object), 0);
	return arikkei_type_implements_a (object->klass->klass.implementation.type, type);
}

void *
arikkei_object_get_interface (ArikkeiObject *object, unsigned int type, ArikkeiInterfaceImplementation **impl)
{
	arikkei_return_val_if_fail (object != NULL, NULL);
	arikkei_return_val_if_fail (ARIKKEI_IS_OBJECT (object), NULL);
	return arikkei_instance_get_interface (object, object->klass->klass.implementation.type, type, impl);
}

