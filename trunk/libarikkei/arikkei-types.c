#define __ARIKKEI_TYPES_C__

/*
 * Basic cross-platform functionality
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "arikkei-strlib.h"
#include "arikkei-interface.h"
#include "arikkei-property.h"
#include "arikkei-reference.h"
#include "arikkei-string.h"
#include "arikkei-value.h"
#include "arikkei-utils.h"
#include "arikkei-function-object.h"

#include "arikkei-types.h"

struct _ClassDef {
	unsigned int type;
	unsigned int parent_type;
	const char *name;
	unsigned int instance_size;
};

const struct _ClassDef defs[] = {
	{ ARIKKEI_TYPE_NONE, ARIKKEI_TYPE_NONE, "none", 0 },
	{ ARIKKEI_TYPE_ANY, ARIKKEI_TYPE_NONE , "any", 0 },
	/* True primitives */
	{ ARIKKEI_TYPE_BOOLEAN, ARIKKEI_TYPE_ANY, "bool", sizeof (unsigned int) },
	{ ARIKKEI_TYPE_INT8, ARIKKEI_TYPE_ANY , "i8", 1 },
	{ ARIKKEI_TYPE_UINT8, ARIKKEI_TYPE_ANY, "u8", 1 },
	{ ARIKKEI_TYPE_INT16, ARIKKEI_TYPE_ANY , "i16", 2 },
	{ ARIKKEI_TYPE_UINT16, ARIKKEI_TYPE_ANY , "u16", 2 },
	{ ARIKKEI_TYPE_INT32, ARIKKEI_TYPE_ANY , "i32", 4 },
	{ ARIKKEI_TYPE_UINT32, ARIKKEI_TYPE_ANY , "u32", 4 },
	{ ARIKKEI_TYPE_INT64, ARIKKEI_TYPE_ANY , "i64", 8 },
	{ ARIKKEI_TYPE_UINT64, ARIKKEI_TYPE_ANY , "u64", 8 },
	{ ARIKKEI_TYPE_FLOAT, ARIKKEI_TYPE_ANY, "float", 4 },
	{ ARIKKEI_TYPE_DOUBLE, ARIKKEI_TYPE_ANY, "double", 8 },
	{ ARIKKEI_TYPE_STRUCT, ARIKKEI_TYPE_ANY, "struct", 0 },

	{ ARIKKEI_TYPE_POINTER, ARIKKEI_TYPE_ANY, "pointer", sizeof (void *) },

	{ ARIKKEI_TYPE_CLASS, ARIKKEI_TYPE_STRUCT, "class", sizeof (ArikkeiClass) },
	{ ARIKKEI_TYPE_INTERFACE, ARIKKEI_TYPE_STRUCT, "interface", 0 },
	{ ARIKKEI_TYPE_REFERENCE, ARIKKEI_TYPE_STRUCT, "reference", sizeof (ArikkeiReference) },
	{ ARIKKEI_TYPE_STRING, ARIKKEI_TYPE_REFERENCE, "string", sizeof (ArikkeiString) }
};

static unsigned int classes_size = 0;
static unsigned int nclasses = 0;
static ArikkeiClass **classes = NULL;

static unsigned int
arikkei_any_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	char c[32];
	unsigned int tlen, nlen, alen;
	unsigned int l;
	/* Instance of NAME 0xADDRESS */
	tlen = 12;
	nlen = strlen ((const char *) klass->name);
	sprintf (c, "%p", instance);
	alen = strlen (c);
	l = (tlen < len) ? tlen : len;
	if (l > 0) memcpy (buf, "Instance of ", l);
	buf += l;
	len -= l;
	l = (nlen < len) ? nlen : len;
	if (l > 0) memcpy (buf, klass->name, l);
	buf += l;
	len -= l;
	if (len > 0) {
		*buf = ' ';
		buf += 1;
		len -= 1;
	}
	l = (alen < len) ? alen : len;
	if (l > 0) memcpy (buf, c, l);
	buf += l;
	len -= l;
	if (len > 0) {
		*buf = 0;
	}
	return tlen + nlen + 1 + alen;
}

static unsigned int
arikkei_float_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	unsigned int pos = 0;
	pos += arikkei_dtoa_exp (buf + pos, len - pos, *((float *) instance), 6, 0);
	if (pos < len) buf[pos] = 0;
	return pos;
}

static unsigned int
arikkei_string_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	unsigned int pos = 0;
	if (instance) {
		ArikkeiString *str = (ArikkeiString *) instance;
		unsigned int slen = (str->length > len) ? len : str->length;
		memcpy (buf + pos, str->str, slen);
		pos += slen;
	}
	if (pos < len) buf[pos] = 0;
	return pos;
}

static void
arikkei_reference_instance_init (void *instance)
{
	ArikkeiReference *ref = ARIKKEI_REFERENCE(instance);
	ref->refcount = 1;
}

void arikkei_types_init (void)
{
	int i;
	if (classes) return;
	classes_size = 32;
	classes = (ArikkeiClass **) malloc (classes_size * sizeof (ArikkeiClass *));
	/* NONE */
	for (i = 0; i < ARIKKEI_TYPE_NUM_PRIMITIVES; i++) {
		if (i == ARIKKEI_TYPE_INTERFACE) {
			classes[i] = (ArikkeiClass *) malloc (sizeof (ArikkeiInterfaceClass));
			memset (classes[i], 0, sizeof (ArikkeiInterfaceClass));
		} else {
			classes[i] = (ArikkeiClass *) malloc (sizeof (ArikkeiClass));
			memset (classes[i], 0, sizeof (ArikkeiClass));
		}
		if (defs[i].parent_type) {
			memcpy (classes[i], classes[defs[i].parent_type], classes[defs[i].parent_type]->class_size);
			classes[i]->parent = classes[defs[i].parent_type];
		}
		classes[i]->type = defs[i].type;
		classes[i]->name = (const unsigned char *) defs[i].name;
		classes[i]->class_size = sizeof (ArikkeiClass);
		classes[i]->instance_size = defs[i].instance_size;
		classes[i]->element_size = classes[i]->instance_size;
		classes[i]->zero_memory = 0;

		if (i == ARIKKEI_TYPE_ANY) classes[i]->to_string = arikkei_any_to_string;
		if (i == ARIKKEI_TYPE_FLOAT) classes[i]->to_string = arikkei_float_to_string;
		if (i == ARIKKEI_TYPE_STRING) classes[i]->to_string = arikkei_string_to_string;
		if (i == ARIKKEI_TYPE_REFERENCE) classes[i]->instance_init = arikkei_reference_instance_init;
		if (i == ARIKKEI_TYPE_INTERFACE) {
			((ArikkeiInterfaceClass *) classes[i])->implementation_size = sizeof (ArikkeiInterfaceImplementation);
		}
		nclasses += 1;
	}
}

static void *
allocate_default (ArikkeiClass *klass)
{
	return malloc (klass->instance_size);
}

static void *
allocate_array_default (ArikkeiClass *klass, unsigned int nelements)
{
	return malloc (nelements * klass->element_size);
}

static void
free_default (ArikkeiClass *klass, void *location)
{
	free (location);
}

static void 
free_array_default (ArikkeiClass *klass, void *location, unsigned int nelements)
{
	free (location);
}

static void
duplicate_default (ArikkeiClass *klass, void *destination, void *instance)
{
	memcpy (destination, instance, klass->instance_size);
}

static void
assign_default (ArikkeiClass *klass, void *destination, void *instance)
{
	klass->instance_finalize (destination);
	memcpy (destination, instance, klass->instance_size);
}

void
arikkei_register_class (unsigned int *type, ArikkeiClass *klass, unsigned int parent, const unsigned char *name, unsigned int class_size, unsigned int instance_size,
						void (* class_init) (ArikkeiClass *), void (* instance_init) (void *), void (* instance_finalize) (void *))
{
	if (!classes) arikkei_types_init ();
	arikkei_return_if_fail (klass != NULL);
	arikkei_return_if_fail ((parent == ARIKKEI_TYPE_NONE) || (class_size >= classes[parent]->class_size));
	arikkei_return_if_fail ((parent == ARIKKEI_TYPE_NONE) || (instance_size >= classes[parent]->instance_size));
	if (nclasses >= classes_size) {
		classes_size += 32;
		classes = (ArikkeiClass **) realloc (classes, classes_size * sizeof (ArikkeiClass *));
	}
	*type = nclasses;
	nclasses += 1;

	classes[*type] = klass;
	memset (klass, 0, class_size);
	if (parent == ARIKKEI_TYPE_NONE) {
		/* Default methods */
		/* Memory management */
		klass->allocate = allocate_default;
		klass->allocate_array = allocate_array_default;
		klass->free = free_default;
		klass->free_array = free_array_default;
	} else {
		memcpy (klass, classes[parent], classes[parent]->class_size);
		klass->parent = classes[parent];
		klass->firstinterface = klass->parent->firstinterface + klass->parent->ninterfaces;
		klass->ninterfaces = 0;
		klass->implementations = NULL;
		klass->firstproperty = klass->parent->firstproperty + klass->parent->nproperties;
		klass->nproperties = 0;
		klass->properties = NULL;
	}
	klass->type = *type;
	klass->name = (unsigned char *) strdup ((const char *) name);
	/* Sizes */
	klass->class_size = class_size;
	klass->instance_size = instance_size;
	klass->element_size = instance_size;
	/* Constructors and destructors */
	klass->instance_init = instance_init;
	klass->instance_finalize = instance_finalize;

	if (class_init) class_init (klass);
}

ArikkeiClass *
arikkei_register_type (unsigned int *type, unsigned int parent, const unsigned char *name, unsigned int class_size, unsigned int instance_size,
				       void (* class_init) (ArikkeiClass *),
					   void (* instance_init) (void *),
					   void (* instance_finalize) (void *))
{
	ArikkeiClass *klass;
	if (!classes) arikkei_types_init ();
	arikkei_return_val_if_fail ((parent == ARIKKEI_TYPE_NONE) || (class_size >= classes[parent]->class_size), NULL);
	arikkei_return_val_if_fail ((parent == ARIKKEI_TYPE_NONE) || (instance_size >= classes[parent]->instance_size), NULL);
	klass = (ArikkeiClass *) malloc (class_size);
	arikkei_register_class (type, klass, parent, name, class_size, instance_size, class_init, instance_init, instance_finalize);
	return klass;
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
	while (klass) {
		if (klass->type == test) return 1;
		klass = klass->parent;
	}
	return 0;
}

unsigned int
arikkei_type_implements_a (unsigned int type, unsigned int test)
{
	arikkei_return_val_if_fail (type < nclasses, 0);
	arikkei_return_val_if_fail (test < nclasses, 0);
	return arikkei_class_get_interface_implementation (classes[type], test) != NULL;
}

unsigned int
arikkei_type_is_assignable_to (unsigned int type, unsigned int test)
{
	if (arikkei_type_is_a (test, ARIKKEI_TYPE_INTERFACE)) {
		unsigned int val = arikkei_type_implements_a (type, test);
		return val;
	} else {
		return arikkei_type_is_a (type, test);
	}
}

unsigned int
arikkei_class_is_of_type (ArikkeiClass *klass, unsigned int type)
{
	arikkei_return_val_if_fail (klass != NULL, 0);
	while (klass) {
		if (klass->type == type) return 1;
		klass = klass->parent;
	}
	return 0;
}

static void
arikkei_class_tree_interface_invoke_init (ArikkeiInterfaceClass *klass, ArikkeiInterfaceImplementation *implementation,  void *instance)
{
	/* Every interface has to be subclass of ArikkeiInterface */
	if (klass->klass.parent && (klass->klass.parent->type >= ARIKKEI_TYPE_INTERFACE)) {
		arikkei_class_tree_interface_invoke_init ((ArikkeiInterfaceClass *) klass->klass.parent, implementation, instance);
	}
	if (klass->klass.instance_init) klass->klass.instance_init (instance);
	if (klass->instance_init) klass->instance_init (implementation, instance);
}

static void
arikkei_class_tree_instance_invoke_init (ArikkeiClass *klass, void *instance)
{
	unsigned int i;
	if (klass->parent && (klass->parent->type != ARIKKEI_TYPE_NONE)) {
		arikkei_class_tree_instance_invoke_init (klass->parent, instance);
	}
	if (klass->instance_init) klass->instance_init (instance);
	for (i = 0; i < klass->ninterfaces; i++) {
		ArikkeiInterfaceImplementation *impl = (ArikkeiInterfaceImplementation *) ((char *) klass + klass->implementations[i]);
		arikkei_class_tree_interface_invoke_init (impl->klass, impl, (char *) instance + impl->instance_offset);
	}
}

static void
arikkei_class_tree_instance_invoke_finalize (ArikkeiClass *klass, void *instance)
{
	unsigned int i;
	if (klass->instance_finalize) klass->instance_finalize (instance);
	for (i = 0; i < klass->ninterfaces; i++) {
		ArikkeiInterfaceImplementation *impl = (ArikkeiInterfaceImplementation *) ((char *) klass + klass->implementations[i]);
		ArikkeiInterfaceClass *ifclass = (ArikkeiInterfaceClass *) impl->klass;
		/* Every interface has to be subclass of ArikkeiInterface */
		while (ifclass->klass.type >= ARIKKEI_TYPE_INTERFACE) {
			if (ifclass->instance_finalize) ifclass->instance_finalize (impl, (char *) instance + impl->instance_offset);
			if (ifclass->klass.instance_finalize) ifclass->klass.instance_finalize ((char *) instance + impl->instance_offset);
			ifclass = (ArikkeiInterfaceClass *) ifclass->klass.parent;
		}
	}
	if (klass->parent && (klass->parent->type != ARIKKEI_TYPE_NONE)) {
		arikkei_class_tree_instance_invoke_finalize (klass->parent, instance);
	}
}

static void
arikkei_class_tree_implementation_invoke_init (ArikkeiInterfaceClass *interface_class, ArikkeiInterfaceImplementation *implementation)
{
	ArikkeiClass *klass = (ArikkeiClass *) interface_class;
	if (klass->parent && (klass->parent->type >= ARIKKEI_TYPE_INTERFACE)) {
		arikkei_class_tree_implementation_invoke_init ((ArikkeiInterfaceClass *) klass->parent, implementation);
	}
	if (interface_class->implementation_init) interface_class->implementation_init (implementation);
}

void *
arikkei_instance_new (unsigned int type)
{
	void *instance;
	ArikkeiClass *klass = arikkei_type_get_class (type);
	arikkei_return_val_if_fail (klass != NULL, NULL);
	instance = klass->allocate (klass);
	memset (instance, 0, klass->instance_size);
	arikkei_class_tree_instance_invoke_init (klass, instance);
	return instance;
}

void *
arikkei_instance_new_array (unsigned int type, unsigned int nelements)
{
	void *elements;
	unsigned int i;
	ArikkeiClass *klass = arikkei_type_get_class (type);
	arikkei_return_val_if_fail (klass != NULL, NULL);
	elements = klass->allocate_array (klass, nelements);
	memset (elements, 0, nelements * klass->element_size);
	for (i = 0; i < nelements; i++) {
		void *instance = (char *) elements + i * klass->element_size;
		arikkei_class_tree_instance_invoke_init (klass, instance);
	}
	return elements;
}

void
arikkei_instance_delete (unsigned int type, void *instance)
{
	ArikkeiClass *klass = arikkei_type_get_class (type);
	arikkei_return_if_fail (klass != NULL);
	arikkei_class_tree_instance_invoke_finalize (klass, instance);
}

void
arikkei_instance_delete_array (unsigned int type, void *elements, unsigned int nelements)
{
	unsigned int i;
	ArikkeiClass *klass = arikkei_type_get_class (type);
	arikkei_return_if_fail (klass != NULL);
	for (i = 0; i < nelements; i++) {
		void *instance = (char *) elements + i * klass->element_size;
		arikkei_class_tree_instance_invoke_finalize (klass, instance);
	}
}

void
arikkei_instance_setup (void *instance, unsigned int type)
{
	ArikkeiClass *klass = arikkei_type_get_class (type);
	arikkei_return_if_fail (klass != NULL);
	memset (instance, 0, klass->instance_size);
	arikkei_class_tree_instance_invoke_init (klass, instance);
}

void
arikkei_instance_release (void *instance, unsigned int type)
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

unsigned int
arikkei_instance_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	if (klass->to_string) {
		return klass->to_string (klass, instance, buf, len);
	}
	return 0;
}

ArikkeiProperty *
arikkei_class_lookup_property (ArikkeiClass *klass, const unsigned char *key)
{
	unsigned int i;
	arikkei_return_val_if_fail (klass != NULL, NULL);
	arikkei_return_val_if_fail (key != NULL, NULL);
	for (i = 0; i < klass->nproperties; i++) {
		if (!strcmp ((const char *) key, (const char *) klass->properties[i].key->str)) return &klass->properties[i];
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
		if (val && !(arikkei_type_is_a (val->type, klass->properties[idx].type) || arikkei_type_implements_a (val->type, klass->properties[idx].type))) return 0;
		if (klass->set_property) {
			return klass->set_property (klass, instance, idx, val);
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
		if (klass->properties[idx].is_value) {
			arikkei_value_copy (val, &klass->properties[idx].value);
			return 1;
		} else if (klass->get_property) {
			return klass->get_property (klass, instance, idx, val);
		}
		return 0;
	} else {
		if (klass->parent) {
			return arikkei_instance_get_property_by_id (klass->parent, instance, id, val);
		}
		return 0;
	}
}

ArikkeiInterfaceImplementation *
arikkei_class_get_interface_implementation (ArikkeiClass *klass, unsigned int type)
{
	ArikkeiClass *ref;
	arikkei_return_val_if_fail (klass != NULL, NULL);
	ref = klass;
	while (ref) {
		unsigned int i;
		for (i = 0; i < ref->ninterfaces; i++) {
			ArikkeiInterfaceImplementation *impl = (ArikkeiInterfaceImplementation *) ((char *) klass + ref->implementations[i]);
			if (arikkei_class_is_of_type ((ArikkeiClass *) impl->klass, type)) {
				return impl;
			}
		}
		ref = ref->parent;
	}
	return NULL;
}

void
arikkei_class_set_num_interfaces (ArikkeiClass *klass, unsigned int ninterfaces)
{
	if (klass->parent) klass->firstinterface = klass->parent->firstinterface + klass->parent->ninterfaces;
	klass->ninterfaces = ninterfaces;
	klass->implementations = (unsigned int *) malloc (ninterfaces * sizeof (unsigned int));
	memset (klass->implementations, 0, ninterfaces * sizeof (unsigned int));
}

void
arikkei_interface_implementation_setup (ArikkeiClass *klass, unsigned int idx, unsigned int type, unsigned int class_offset, unsigned int instance_offset)
{
	ArikkeiInterfaceClass *interface_class;
	ArikkeiInterfaceImplementation *impl;
	arikkei_return_if_fail (klass != NULL);
	arikkei_return_if_fail (idx < klass->ninterfaces);
	arikkei_return_if_fail (arikkei_type_is_a (type, ARIKKEI_TYPE_INTERFACE));
	interface_class = (ArikkeiInterfaceClass *) arikkei_type_get_class (type);
	klass->implementations[idx] = class_offset;
	impl = (ArikkeiInterfaceImplementation *) ((char *) klass + class_offset);
	impl->klass = interface_class;
	impl->class_offset = class_offset;
	impl->instance_offset = instance_offset;
	arikkei_class_tree_implementation_invoke_init (interface_class, impl);
}

void
arikkei_class_set_num_properties (ArikkeiClass *klass, unsigned int nproperties)
{
	if (klass->parent) klass->firstproperty = klass->parent->firstproperty + klass->parent->nproperties;
	klass->nproperties = nproperties;
	klass->properties = (ArikkeiProperty *) malloc (nproperties * sizeof (ArikkeiProperty));
	memset (klass->properties, 0, nproperties * sizeof (ArikkeiProperty));
}

void
arikkei_class_property_setup (ArikkeiClass *klass, unsigned int idx, const unsigned char *key, unsigned int type,
							  unsigned int is_static, unsigned int can_read, unsigned int can_write, unsigned int is_final, unsigned int is_value,
							  unsigned int value_type, void *value)
{
	arikkei_return_if_fail (klass != NULL);
	arikkei_return_if_fail (idx < klass->nproperties);
	arikkei_return_if_fail (key != NULL);
	arikkei_return_if_fail (type != ARIKKEI_TYPE_NONE);
	arikkei_return_if_fail (!(can_write && is_final));
	arikkei_return_if_fail (!is_value || is_static);
	if (!((value_type == ARIKKEI_TYPE_NONE) || (arikkei_type_is_assignable_to (value_type, type)))) {
		return;
	}
	arikkei_return_if_fail ((value_type == ARIKKEI_TYPE_NONE) || (arikkei_type_is_assignable_to (value_type, type)));

	arikkei_property_setup (klass->properties + idx, key, type, klass->firstproperty + idx, is_static, can_read, can_write, is_final, is_value, value_type, value);
}

void
arikkei_class_method_setup (ArikkeiClass *klass, unsigned int idx, const unsigned char *key,
							unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
							unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *))
{
	ArikkeiFunctionObject *fobj;
	fobj = arikkei_function_object_new (klass->type, rettype, nargs, argtypes, call);
	/* Property is static although function is not static */
	arikkei_class_property_setup (klass, idx, key, ARIKKEI_TYPE_FUNCTION, 1, 1, 0, 1, 1, ARIKKEI_TYPE_FUNCTION_OBJECT, fobj);
	arikkei_object_unref ((ArikkeiObject *) fobj);
}

void
arikkei_class_static_method_setup (ArikkeiClass *klass, unsigned int idx, const unsigned char *key,
								   unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
								   unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *))
{
	ArikkeiFunctionObject *fobj;
	fobj = arikkei_function_object_new (ARIKKEI_TYPE_NONE, rettype, nargs, argtypes, call);
	arikkei_class_property_setup (klass, idx, key, ARIKKEI_TYPE_FUNCTION, 1, 1, 0, 1, 1, ARIKKEI_TYPE_FUNCTION_OBJECT, fobj);
	arikkei_object_unref ((ArikkeiObject *) fobj);
}
