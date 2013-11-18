#define __ARIKKEI_TYPES_C__

/*
 * Basic cross-platform functionality
 */

#include <string.h>
#include <stdlib.h>

#include "arikkei-value.h"
#include "arikkei-utils.h"

#include "arikkei-types.h"

struct _ClassDef {
	unsigned int type;
	unsigned int parent_type;
	const char *name;
	unsigned int instance_size;
};

const struct _ClassDef defs[] = {
	{ ARIKKEI_TYPE_NONE, ARIKKEI_TYPE_NONE, "none", 0 },
	{ ARIKKEI_TYPE_BOOLEAN, ARIKKEI_TYPE_NONE, "bool", sizeof (unsigned int) },
	{ ARIKKEI_TYPE_INT8, ARIKKEI_TYPE_NONE , "i8", 1 },
	{ ARIKKEI_TYPE_UINT8, ARIKKEI_TYPE_NONE, "u8", 1 },
	{ ARIKKEI_TYPE_INT16, ARIKKEI_TYPE_NONE , "i16", 2 },
	{ ARIKKEI_TYPE_UINT16, ARIKKEI_TYPE_NONE , "u16", 2 },
	{ ARIKKEI_TYPE_INT32, ARIKKEI_TYPE_NONE , "i32", 4 },
	{ ARIKKEI_TYPE_UINT32, ARIKKEI_TYPE_NONE , "u32", 4 },
	{ ARIKKEI_TYPE_INT64, ARIKKEI_TYPE_NONE , "i64", 8 },
	{ ARIKKEI_TYPE_UINT64, ARIKKEI_TYPE_NONE , "u64", 8 },
	{ ARIKKEI_TYPE_FLOAT, ARIKKEI_TYPE_NONE, "float", 4 },
	{ ARIKKEI_TYPE_DOUBLE, ARIKKEI_TYPE_NONE, "double", 8 },
	{ ARIKKEI_TYPE_POINTER, ARIKKEI_TYPE_NONE, "pointer", sizeof (void *) },
	{ ARIKKEI_TYPE_POINTER, ARIKKEI_TYPE_NONE, "struct", 4 },
	{ ARIKKEI_TYPE_STRING, ARIKKEI_TYPE_NONE, "string", sizeof (ArikkeiString *) }
};

static unsigned int classes_size = 0;
static unsigned int nclasses = 0;
static ArikkeiClass **classes = NULL;

void arikkei_types_init (void)
{
	int i;
	if (classes) return;
	classes_size = 32;
	classes = (ArikkeiClass **) malloc (classes_size * sizeof (ArikkeiClass *));
	for (i = 0; i < ARIKKEI_TYPE_NUM_PRIMITIVES; i++) {
		classes[i] = (ArikkeiClass *) malloc (sizeof (ArikkeiClass));
		memset (classes[i], 0, sizeof (ArikkeiClass));
		classes[i]->type = defs[i].type;
		classes[i]->parent = classes[defs[i].parent_type];
		classes[i]->name = (const unsigned char *) defs[i].name;
		classes[i]->instance_size = defs[i].instance_size;
		nclasses += 1;
	}
}

void
arikkei_register_type (unsigned int *type, unsigned int parent, const unsigned char *name, unsigned int class_size, unsigned int instance_size,
				       void (* class_init) (ArikkeiClass *),
					   void (* instance_init) (void *),
					   void (* instance_finalize) (void *))
{
	ArikkeiClass *klass;

	if (!classes) arikkei_types_init ();

	if (nclasses >= classes_size) {
		classes_size += 32;
		classes = (ArikkeiClass **) realloc (classes, classes_size * sizeof (ArikkeiClass *));
#if 0
		if (classes_len == 0) {
			classes[0] = NULL;
			classes_len = 1;
		}
#endif
	}

	*type = nclasses;
	nclasses += 1;

	classes[*type] = (ArikkeiClass *) malloc (class_size);
	klass = classes[*type];
	memset (klass, 0, class_size);

	if (parent != ARIKKEI_TYPE_NONE) {
		memcpy (klass, classes[parent], classes[parent]->class_size);
		klass->parent = classes[parent];
		klass->firstproperty = klass->parent->firstproperty + klass->parent->nproperties;
		klass->nproperties = 0;
		klass->properties = NULL;
	}
	klass->type = *type;
	klass->name = (unsigned char *) strdup ((const char *) name);
	klass->class_size = class_size;
	klass->instance_size = instance_size;
	klass->class_init = class_init;
	klass->instance_init = instance_init;
	klass->instance_finalize = instance_finalize;

	if (klass->class_init) klass->class_init (klass);
}

ArikkeiClass *
arikkei_type_get_class (unsigned int type)
{
	return classes[type];
}

unsigned int
arikkei_type_is_a (unsigned int type, unsigned int test)
{
	ArikkeiClass *klass;
	arikkei_return_val_if_fail (type < nclasses, 0);
	arikkei_return_val_if_fail (test < nclasses, 0);
	if (type == test) return 1;
	klass = classes[type]->parent;
	while (klass->type != ARIKKEI_TYPE_NONE) {
		if (klass->type == test) return 1;
		klass = klass->parent;
	}
	return 0;
}

unsigned int
arikkei_class_is_of_type (ArikkeiClass *klass, unsigned int type)
{
	arikkei_return_val_if_fail (klass != NULL, 0);
	while (klass->type != ARIKKEI_TYPE_NONE) {
		if (klass->type == type) return 1;
		klass = klass->parent;
	}
	return 0;
}

static void
arikkei_class_tree_instance_invoke_init (ArikkeiClass *klass, void *instance)
{
	if (klass->parent && (klass->parent->type != ARIKKEI_TYPE_NONE)) {
		arikkei_class_tree_instance_invoke_init (klass->parent, instance);
	}
	if (klass->instance_init) klass->instance_init (instance);
}

static void
arikkei_class_tree_instance_invoke_finalize (ArikkeiClass *klass, void *instance)
{
	if (klass->instance_finalize) klass->instance_finalize (instance);
	if (klass->parent && (klass->parent->type != ARIKKEI_TYPE_NONE)) {
		arikkei_class_tree_instance_invoke_finalize (klass->parent, instance);
	}
}

void
arikkei_type_setup_instance (void *instance, unsigned int type)
{
	ArikkeiClass *klass;
	klass = arikkei_type_get_class (type);
	arikkei_return_if_fail (klass != NULL);
	memset (instance, 0, klass->instance_size);
	arikkei_class_tree_instance_invoke_init (klass, instance);
}

void
arikkei_type_release_instance (void *instance, unsigned int type)
{
	ArikkeiClass *klass;
	klass = arikkei_type_get_class (type);
	arikkei_return_if_fail (klass != NULL);
	arikkei_class_tree_instance_invoke_finalize (klass, instance);
}

const unsigned char *
arikkei_type_get_name (unsigned int type)
{
	return classes[type]->name;
}

ArikkeiProperty *
arikkei_class_lookup_property (ArikkeiClass *klass, const unsigned char *key)
{
	unsigned int i;
	arikkei_return_val_if_fail (klass != NULL, NULL);
	arikkei_return_val_if_fail (key != NULL, NULL);
	for (i = 0; i < klass->nproperties; i++) {
		if (!strcmp ((const char *) key, (const char *) klass->properties[i].key)) return &klass->properties[i];
	}
	if (klass->parent) {
		return arikkei_class_lookup_property (klass->parent, key);
	}
	return NULL;
}

unsigned int
arikkei_instance_set_property (ArikkeiClass *klass, void *instance, const unsigned char *key, const ArikkeiValue *val)
{
	ArikkeiProperty *prop;
	arikkei_return_val_if_fail (klass != NULL, 0);
	arikkei_return_val_if_fail (key != NULL, 0);
	arikkei_return_val_if_fail (val != NULL, 0);
	prop = arikkei_class_lookup_property (klass, key);
	if (!prop) return 0;
	return arikkei_instance_set_property_by_id (klass, instance, prop->id, val);
}

unsigned int
arikkei_instance_set_property_by_id (ArikkeiClass *klass, void *instance, unsigned int id, const ArikkeiValue *val)
{
	arikkei_return_val_if_fail (klass != NULL, 0);
	arikkei_return_val_if_fail (val != NULL, 0);
	if (id >= klass->firstproperty) {
		unsigned int idx = id - klass->firstproperty;
		if (!klass->properties[idx].can_write) return 0;
		if (klass->properties[idx].is_final) return 0;
		if (val && !arikkei_type_is_a (val->type, klass->properties[idx].type)) return 0;
		if (klass->set_property) {
			return klass->set_property (instance, idx, val);
		}
		return 0;
	} else {
		if (klass->parent) {
			return arikkei_instance_set_property_by_id (klass->parent, instance, id, val);
		}
		return 0;
	}
}

unsigned int
arikkei_instance_get_property (ArikkeiClass *klass, void *instance, const unsigned char *key, ArikkeiValue *val)
{
	ArikkeiProperty *prop;
	arikkei_return_val_if_fail (klass != NULL, 0);
	arikkei_return_val_if_fail (key != NULL, 0);
	arikkei_return_val_if_fail (val != NULL, 0);
	prop = arikkei_class_lookup_property (klass, key);
	if (!prop) return 0;
	return arikkei_instance_get_property_by_id (klass, instance, prop->id, val);
}

unsigned int
arikkei_instance_get_property_by_id (ArikkeiClass *klass, void *instance, unsigned int id, ArikkeiValue *val)
{
	arikkei_return_val_if_fail (klass != NULL, 0);
	arikkei_return_val_if_fail (val != NULL, 0);
	if (id >= klass->firstproperty) {
		unsigned int idx = id - klass->firstproperty;
		if (!klass->properties[idx].can_read) return 0;
		if (klass->get_property) {
			return klass->get_property (instance, idx, val);
		}
		return 0;
	} else {
		if (klass->parent) {
			return arikkei_instance_get_property_by_id (klass->parent, instance, id, val);
		}
		return 0;
	}
}

void *
arikkei_instance_get_interface (ArikkeiClass *klass, void *instance, unsigned int type)
{
	arikkei_return_val_if_fail (klass != NULL, NULL);
	if (klass->type == type) return instance;
	if (klass->get_interface) {
		return klass->get_interface (instance, type);
	}
	if (klass->parent) {
		return klass->parent->get_interface (instance, type);
	}
	return NULL;
}

void
arikkei_property_setup (ArikkeiProperty *prop, const unsigned char *key, unsigned int type, unsigned int id, unsigned int isstatic, unsigned int canread, unsigned int canwrite, unsigned int isfinal, void *value)
{
	prop->key = key;
	prop->type = type;
	prop->id = id;
	prop->is_static = isstatic;
	prop->can_read = canread;
	prop->can_write = canwrite;
	prop->is_final = isfinal;
	if (value) {
		arikkei_value_set (&prop->defval, type, value);
	} else {
		prop->type = type;
	}
}

void
arikkei_class_set_num_properties (ArikkeiClass *klass, unsigned int nproperties)
{
	if (klass->parent) klass->firstproperty = klass->parent->firstproperty + klass->parent->nproperties;
	klass->nproperties = nproperties;
	klass->properties = (ArikkeiProperty *) malloc (nproperties * sizeof (ArikkeiProperty));
	memset (klass->properties, 0, nproperties * sizeof (ArikkeiProperty));
}
