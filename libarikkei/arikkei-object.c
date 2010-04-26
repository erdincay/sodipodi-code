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

#ifdef WIN32
#define strdup _strdup
#endif

/* #include <libnr/nr-macros.h> */

#include "arikkei-object.h"

unsigned int
arikkei_emit_fail_warning (const unsigned char *file, unsigned int line, const unsigned char *method, const unsigned char *expr)
{
	fprintf (stderr, "File %s line %d (%s): Assertion %s failed\n", file, line, method, expr);
	return 1;
}

/* ArikkeiObject */

static ArikkeiObjectClass **classes = NULL;
static unsigned int classes_len = 0;
static unsigned int classes_size = 0;

unsigned int
arikkei_type_is_a (unsigned int type, unsigned int test)
{
	ArikkeiObjectClass *klass;

	arikkei_return_val_if_fail (type < classes_len, 0);
	arikkei_return_val_if_fail (test < classes_len, 0);

	klass = classes[type];

	while (klass) {
		if (klass->type == test) return 1;
		klass = klass->parent;
	}

	return 0;
}

void *
arikkei_object_check_instance_cast (void *ip, unsigned int tc)
{
	arikkei_return_val_if_fail (ip != NULL, NULL);
	arikkei_return_val_if_fail (arikkei_type_is_a (((ArikkeiObject *) ip)->klass->type, tc), ip);
	return ip;
}

unsigned int
arikkei_object_check_instance_type (void *ip, unsigned int tc)
{
	if (ip == NULL) return 0;
	return arikkei_type_is_a (((ArikkeiObject *) ip)->klass->type, tc);
}

unsigned int
arikkei_object_register_type (unsigned int parent,
			 const unsigned char *name,
			 unsigned int csize,
			 unsigned int isize,
			 void (* cinit) (ArikkeiObjectClass *),
			 void (* iinit) (ArikkeiObject *))
{
	unsigned int type;
	ArikkeiObjectClass *klass;

	if (classes_len >= classes_size) {
		classes_size += 32;
		classes = (ArikkeiObjectClass **) realloc (classes, classes_size * sizeof (ArikkeiObjectClass *));
		if (classes_len == 0) {
			classes[0] = NULL;
			classes_len = 1;
		}
	}

	type = classes_len;
	classes_len += 1;

	classes[type] = malloc (csize);
	klass = classes[type];
	memset (klass, 0, csize);

	if (classes[parent]) {
		memcpy (klass, classes[parent], classes[parent]->csize);
	}

	klass->type = type;
	klass->parent = classes[parent];
	klass->name = (unsigned char *) strdup ((const char *) name);
	klass->csize = csize;
	klass->isize = isize;
	klass->cinit = cinit;
	klass->iinit = iinit;

	klass->cinit (klass);

	return type;
}

static void arikkei_object_class_init (ArikkeiObjectClass *klass);
static void arikkei_object_init (ArikkeiObject *object);
static void arikkei_object_finalize (ArikkeiObject *object);

unsigned int
arikkei_object_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (0,
						(const unsigned char *) "ArikkeiObject",
						sizeof (ArikkeiObjectClass),
						sizeof (ArikkeiObject),
						(void (*) (ArikkeiObjectClass *)) arikkei_object_class_init,
						(void (*) (ArikkeiObject *)) arikkei_object_init);
	}
	return type;
}

static void
arikkei_object_class_init (ArikkeiObjectClass *klass)
{
	klass->finalize = arikkei_object_finalize;
}

static void arikkei_object_init (ArikkeiObject *object)
{
}

static void arikkei_object_finalize (ArikkeiObject *object)
{
}

/* Dynamic lifecycle */

ArikkeiObject *
arikkei_object_new (unsigned int type)
{
	ArikkeiObjectClass *klass;
	ArikkeiObject *object;

	arikkei_return_val_if_fail (type < classes_len, NULL);

	klass = classes[type];
	object = malloc (klass->isize);
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

ArikkeiObject *
arikkei_object_ref (ArikkeiObject *object)
{
	object->refcount += 1;
	return object;
}

ArikkeiObject *
arikkei_object_unref (ArikkeiObject *object)
{
	object->refcount -= 1;
	if (object->refcount < 1) {
		arikkei_object_delete (object);
	}
	return NULL;
}

/* Automatic lifecycle */

static void
arikkei_class_tree_object_invoke_init (ArikkeiObjectClass *klass, ArikkeiObject *object)
{
	if (klass->parent) {
		arikkei_class_tree_object_invoke_init (klass->parent, object);
	}
	klass->iinit (object);
}

ArikkeiObject *
arikkei_object_setup (ArikkeiObject *object, unsigned int type)
{
	ArikkeiObjectClass *klass;

	arikkei_return_val_if_fail (type < classes_len, NULL);

	klass = classes[type];

	memset (object, 0, klass->isize);
	object->klass = klass;
	object->refcount = 1;

	arikkei_class_tree_object_invoke_init (klass, object);

	return object;
}

ArikkeiObject *
arikkei_object_release (ArikkeiObject *object)
{
	object->klass->finalize (object);
	return NULL;
}

/* ArikkeiActiveObject */

static void arikkei_active_object_class_init (ArikkeiActiveObjectClass *klass);
static void arikkei_active_object_init (ArikkeiActiveObject *object);
static void arikkei_active_object_finalize (ArikkeiObject *object);

static ArikkeiObjectClass *parent_class;

unsigned int
arikkei_active_object_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		type = arikkei_object_register_type (ARIKKEI_TYPE_OBJECT,
						(const unsigned char *) "ArikkeiActiveObject",
						sizeof (ArikkeiActiveObjectClass),
						sizeof (ArikkeiActiveObject),
						(void (*) (ArikkeiObjectClass *)) arikkei_active_object_class_init,
						(void (*) (ArikkeiObject *)) arikkei_active_object_init);
	}
	return type;
}

static void
arikkei_active_object_class_init (ArikkeiActiveObjectClass *klass)
{
	ArikkeiObjectClass *object_class;

	object_class = (ArikkeiObjectClass *) klass;

	parent_class = ((ArikkeiObjectClass *) klass)->parent;

	object_class->finalize = arikkei_active_object_finalize;
}

static void
arikkei_active_object_init (ArikkeiActiveObject *object)
{
}

static void
arikkei_active_object_finalize (ArikkeiObject *object)
{
	ArikkeiActiveObject *aobject;

	aobject = (ArikkeiActiveObject *) object;

	if (aobject->callbacks) {
		unsigned int i;
		for (i = 0; i < aobject->callbacks->length; i++) {
			ArikkeiObjectListener *listener;
			listener = aobject->callbacks->listeners + i;
			if (listener->vector->dispose) listener->vector->dispose (object, listener->data);
		}
		free (aobject->callbacks);
	}

	((ArikkeiObjectClass *) (parent_class))->finalize (object);
}

void
arikkei_active_object_add_listener (ArikkeiActiveObject *object, const ArikkeiObjectEventVector *vector, unsigned int size, void *data)
{
	ArikkeiObjectListener *listener;

	if (!object->callbacks) {
		object->callbacks = malloc (sizeof (ArikkeiObjectCallbackBlock));
		object->callbacks->size = 1;
		object->callbacks->length = 0;
	}
	if (object->callbacks->length >= object->callbacks->size) {
		int newsize;
		newsize = object->callbacks->size << 1;
		object->callbacks = realloc (object->callbacks, sizeof (ArikkeiObjectCallbackBlock) + (newsize - 1) * sizeof (ArikkeiObjectListener));
		object->callbacks->size = newsize;
	}
	listener = object->callbacks->listeners + object->callbacks->length;
	listener->vector = vector;
	listener->size = size;
	listener->data = data;
	object->callbacks->length += 1;
}

void
arikkei_active_object_remove_listener_by_data (ArikkeiActiveObject *object, void *data)
{
	if (object->callbacks) {
		unsigned int i;
		for (i = 0; i < object->callbacks->length; i++) {
			ArikkeiObjectListener *listener;
			listener = object->callbacks->listeners + i;
			if (listener->data == data) {
				object->callbacks->length -= 1;
				if (object->callbacks->length < 1) {
					free (object->callbacks);
					object->callbacks = NULL;
				} else if (object->callbacks->length != i) {
					*listener = object->callbacks->listeners[object->callbacks->length];
				}
				return;
			}
		}
	}
}


