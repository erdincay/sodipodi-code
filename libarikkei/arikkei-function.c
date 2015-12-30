#define __ARIKKEI_FUNCTION_C__

/*
 * Generic object system
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "arikkei-function.h"

/* ArikkeiFunction */

static void arikkei_function_class_init (ArikkeiFunctionClass *klass);
static void arikkei_function_finalize (ArikkeiInterfaceImplementation *implementation, ArikkeiFunctionInstance *func);

unsigned int
arikkei_function_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		arikkei_register_interface_type (&type, ARIKKEI_TYPE_INTERFACE, (const unsigned char *) "ArikkeiFunction",
						sizeof (ArikkeiFunctionClass), sizeof (ArikkeiFunctionImplementation), sizeof (ArikkeiFunctionInstance),
						NULL,
						NULL,
						NULL, (void (*) (ArikkeiInterfaceImplementation *, void *)) arikkei_function_finalize);
	}
	return type;
}

static void
arikkei_function_finalize (ArikkeiInterfaceImplementation *implementation, ArikkeiFunctionInstance *func)
{
	if (func->argtypes) free (func->argtypes);
}

unsigned int
arikkei_function_check_arguments (ArikkeiFunctionImplementation *implementation, ArikkeiFunctionInstance *func, ArikkeiValue *thisval, ArikkeiValue *args, unsigned int nargs, unsigned int *canconvert)
{
	unsigned int compatible, i;
	arikkei_return_val_if_fail (implementation != NULL, 0);
	arikkei_return_val_if_fail (arikkei_type_is_a(implementation->iface.type, ARIKKEI_TYPE_FUNCTION), 0);
	arikkei_return_val_if_fail (func != NULL, 0);
	if (nargs != func->nargs) {
		if (canconvert) *canconvert = 0;
		return 0;
	}
	if (func->thistype != ARIKKEI_TYPE_NONE) {
		if (!arikkei_type_is_a (thisval->type, func->thistype)) {
			if (!canconvert) return 0;
			if (!arikkei_value_can_convert (func->thistype, thisval)) {
				*canconvert = 0;
				return 0;
			}
		}
	}
	compatible = 1;
	for (i = 0; i < func->nargs; i++) {
		if (!arikkei_type_is_a (args[i].type, func->argtypes[i])) {
			if (!canconvert) return 0;
			if (!arikkei_value_can_convert (func->argtypes[i], &args[i])) {
				*canconvert = 0;
				return 0;
			}
			compatible = 0;
		}
	}
	*canconvert = 1;
	return compatible;
}

unsigned int
arikkei_function_convert_arguments (ArikkeiFunctionImplementation *implementation, ArikkeiFunctionInstance *func, ArikkeiValue *dst, ArikkeiValue *src)
{
	unsigned int i;
	arikkei_return_val_if_fail (implementation != NULL, 0);
	arikkei_return_val_if_fail (arikkei_type_is_a (implementation->iface.type, ARIKKEI_TYPE_FUNCTION), 0);
	arikkei_return_val_if_fail (func != NULL, 0);
	for (i = 0; i < func->nargs; i++) {
		if (!arikkei_value_convert (&dst[i], func->argtypes[i], &src[i])) return 0;
	}
	return 1;
}

unsigned int
arikkei_function_invoke (ArikkeiFunctionImplementation *implementation, ArikkeiFunctionInstance *instance, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args, unsigned int checktypes)
{
	arikkei_return_val_if_fail (implementation != NULL, 0);
	arikkei_return_val_if_fail (arikkei_type_is_a (implementation->iface.type, ARIKKEI_TYPE_FUNCTION), 0);
	arikkei_return_val_if_fail (instance != NULL, 0);
	if (checktypes) {
		unsigned int i;
		if (instance->thistype != ARIKKEI_TYPE_NONE) {
			arikkei_return_val_if_fail (arikkei_type_is_a (thisval->type, instance->thistype), 0);
		}
		arikkei_return_val_if_fail ((instance->rettype == ARIKKEI_TYPE_NONE) || (retval != NULL), 0);
		for (i = 0; i < instance->nargs; i++) {
			arikkei_return_val_if_fail (arikkei_type_is_a (args[i].type, instance->argtypes[i]), 0);
		}
	}
	if (implementation->invoke) {
		return implementation->invoke (implementation, instance, thisval, retval, args);
	}
	return 0;
}

unsigned int
arikkei_function_invoke_direct (ArikkeiFunctionImplementation *implementation, ArikkeiFunctionInstance *instance, ArikkeiValue *thisval, ArikkeiValue *retval, ...)
{
	ArikkeiValue *vals;
	va_list ap;
	unsigned int result, i;
	arikkei_return_val_if_fail (implementation != NULL, 0);
	arikkei_return_val_if_fail (arikkei_type_is_a (implementation->iface.type, ARIKKEI_TYPE_FUNCTION), 0);
	arikkei_return_val_if_fail (instance != NULL, 0);
	if (instance->thistype != ARIKKEI_TYPE_NONE) {
		arikkei_return_val_if_fail (arikkei_type_is_a (thisval->type, instance->thistype), 0);
	}
	arikkei_return_val_if_fail ((instance->rettype == ARIKKEI_TYPE_NONE) || (retval != NULL), 0);
	vals = (ArikkeiValue *) malloc (instance->nargs * sizeof (ArikkeiValue));
	memset (vals, 0, instance->nargs * sizeof (ArikkeiValue));
	va_start (ap, instance->nargs);
	for (i = 0; i < instance->nargs; i++) {
		vals[i].type = instance->argtypes[i];
		switch (instance->argtypes[i]) {
		case ARIKKEI_TYPE_NONE:
			break;
		case ARIKKEI_TYPE_BOOLEAN:
			vals[i].bvalue = va_arg (ap, unsigned int);
			break;
		case ARIKKEI_TYPE_INT8:
		case ARIKKEI_TYPE_UINT8:
		case ARIKKEI_TYPE_INT16:
		case ARIKKEI_TYPE_UINT16:
		case ARIKKEI_TYPE_INT32:
		case ARIKKEI_TYPE_UINT32:
			vals[i].bvalue = va_arg (ap, int);
			break;
		case ARIKKEI_TYPE_INT64:
		case ARIKKEI_TYPE_UINT64:
			vals[i].lvalue = va_arg (ap, long long);
			break;
		case ARIKKEI_TYPE_FLOAT:
			vals[i].fvalue = va_arg (ap, float);
			break;
		case ARIKKEI_TYPE_DOUBLE:
			vals[i].dvalue = va_arg (ap, double);
			break;
		case ARIKKEI_TYPE_POINTER:
			vals[i].pvalue = va_arg (ap, void *);
			break;
		case ARIKKEI_TYPE_STRING:
			arikkei_value_set_string (&vals[i], (ArikkeiString *) va_arg (ap, void *));
			break;
		default:
			arikkei_value_set_object (&vals[i], (ArikkeiObject *) va_arg (ap, void *));
			break;
		}
	}
    va_end (ap);
	result = arikkei_function_invoke (implementation, instance, thisval, retval, vals, 0);
	free (vals);
	return result;
}

unsigned int
arikkei_function_invoke_by_type_instance (unsigned int type, void *instance, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args, unsigned int checktypes)
{
	ArikkeiClass *klass;
	ArikkeiFunctionImplementation *impl;
	ArikkeiFunctionInstance *inst;
	arikkei_return_val_if_fail (arikkei_type_implements_a (type, ARIKKEI_TYPE_FUNCTION), 0);
	arikkei_return_val_if_fail (instance != NULL, 0);
	klass = arikkei_type_get_class (type);
	impl = (ArikkeiFunctionImplementation *) arikkei_class_get_interface_implementation (klass, ARIKKEI_TYPE_FUNCTION);
	inst = (ArikkeiFunctionInstance *) arikkei_interface_get_instance ((ArikkeiInterfaceImplementation *) impl, instance);
	return arikkei_function_invoke (impl, inst, thisval, retval, args, checktypes);
}
