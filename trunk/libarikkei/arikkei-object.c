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

/* ArikkeiInstance implementation */
static void arikkei_object_class_init (ArikkeiObjectClass *klass);
static void arikkei_object_init (ArikkeiObject *object);
/* ArikkeiReference implementation */
void object_dispose (ArikkeiReferenceClass *klass, ArikkeiReference *ref);

unsigned int
arikkei_object_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		arikkei_register_type (&type, ARIKKEI_TYPE_REFERENCE, (const unsigned char *) "ArikkeiObject",
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
	((ArikkeiClass *) klass)->flags |= ARIKKEI_CLASS_ZERO_MEMORY;
	klass->reference_klass.dispose = object_dispose;
}

static void
arikkei_object_init (ArikkeiObject *object)
{
#if 0
	object->refcount = 1;
#endif
	object->flags |= ARIKKEI_OBJECT_ALIVE;
}

void
object_dispose (ArikkeiReferenceClass *klass, ArikkeiReference *ref)
{
	ArikkeiObject *obj = ARIKKEI_OBJECT (ref);
	if (obj->flags & ARIKKEI_OBJECT_ALIVE) {
		if (obj->klass->shutdown) {
			obj->klass->shutdown (obj);
		}
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
arikkei_object_shutdown (ArikkeiObject *obj)
{
	arikkei_return_if_fail (obj != NULL);
	arikkei_return_if_fail (ARIKKEI_IS_OBJECT (obj));
	arikkei_return_if_fail (obj->flags & ARIKKEI_OBJECT_ALIVE);
	if (obj->klass->shutdown) {
		obj->klass->shutdown (obj);
	}
	obj->flags &= ~ARIKKEI_OBJECT_ALIVE;
	arikkei_object_unref (obj);
}

void *
arikkei_object_check_instance_cast (void *ip, unsigned int tc)
{
	if (ip == NULL) {
		fprintf (stderr, "arikkei_object_check_instance_cast: ip == NULL\n");
		return NULL;
	}
	if (!arikkei_type_is_a (((ArikkeiObject *) ip)->klass->reference_klass.klass.implementation.type, tc)) {
		ArikkeiClass *klass = arikkei_type_get_class (tc);
		fprintf (stderr, "arikkei_object_check_instance_cast: %s is not %s\n", ((ArikkeiObject *) ip)->klass->reference_klass.klass.name, klass->name);
		return NULL;
	}
	return ip;
}

unsigned int
arikkei_object_check_instance_type (void *ip, unsigned int tc)
{
	if (ip == NULL) return 0;
	return arikkei_type_is_a (((ArikkeiObject *) ip)->klass->reference_klass.klass.implementation.type, tc);
}

unsigned int
arikkei_object_implements (ArikkeiObject *object, unsigned int type)
{
	arikkei_return_val_if_fail (object != NULL, 0);
	arikkei_return_val_if_fail (ARIKKEI_IS_OBJECT (object), 0);
	return arikkei_type_implements_a (object->klass->reference_klass.klass.implementation.type, type);
}

ArikkeiImplementation *
arikkei_object_get_interface (ArikkeiObject *object, unsigned int type, void **inst)
{
	arikkei_return_val_if_fail (object != NULL, NULL);
	arikkei_return_val_if_fail (ARIKKEI_IS_OBJECT (object), NULL);
	return arikkei_get_interface (&object->klass->reference_klass.klass.implementation, object, type, inst);
}

void
arikkei_value_set_object (ArikkeiValue *val, ArikkeiObject *obj)
{
	if (!obj) {
		arikkei_value_set_reference (val, ARIKKEI_TYPE_OBJECT, (ArikkeiReference *) obj);
	} else {
		arikkei_value_set_reference (val, obj->klass->reference_klass.klass.implementation.type, (ArikkeiReference *) obj);
	}
}
