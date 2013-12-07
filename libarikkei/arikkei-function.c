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
static void arikkei_function_finalize (ArikkeiFunction *func);

/* ArikkeiFunction implementation */
static unsigned int arikkei_function_invoke_default (ArikkeiFunction *func, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args);

static ArikkeiObjectClass *parent_class;

unsigned int
arikkei_function_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		arikkei_register_type (&type, ARIKKEI_TYPE_OBJECT,
						(const unsigned char *) "ArikkeiFunction",
						sizeof (ArikkeiFunctionClass),
						sizeof (ArikkeiFunction),
						(void (*) (ArikkeiClass *)) arikkei_function_class_init,
						NULL,
						(void (*) (void *)) arikkei_function_finalize);
	}
	return type;
}

static void
arikkei_function_class_init (ArikkeiFunctionClass *klass)
{
	parent_class = (ArikkeiObjectClass *) ((ArikkeiClass *) klass)->parent;
	klass->invoke = arikkei_function_invoke_default;
}

static void
arikkei_function_finalize (ArikkeiFunction *func)
{
	if (func->argtypes) free (func->argtypes);
}

static unsigned int
arikkei_function_invoke_default (ArikkeiFunction *func, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args)
{
	if (func->call) {
		return func->call (thisval, retval, args);
	}
	return 0;
}

ArikkeiFunction *
arikkei_function_new (unsigned int thistype, unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
					  unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *))
{
	ArikkeiFunction *func;
	func = (ArikkeiFunction *) arikkei_object_new (ARIKKEI_TYPE_FUNCTION);
	func->thistype = thistype;
	func->rettype = rettype;
	if (nargs) {
		func->nargs = nargs;
		func->argtypes = (unsigned int *) malloc (nargs * sizeof (unsigned int));
		memcpy (func->argtypes, argtypes, nargs * sizeof (unsigned int));
	}
	func->call = call;
	return func;
}

unsigned int
arikkei_function_check_arguments (ArikkeiFunction *func, ArikkeiValue *thisval, ArikkeiValue *args, unsigned int nargs, unsigned int *canconvert)
{
	unsigned int compatible, i;
	arikkei_return_val_if_fail (func != NULL, 0);
	arikkei_return_val_if_fail (ARIKKEI_IS_FUNCTION (func), 0);
	if (nargs != func->nargs) {
		*canconvert = 0;
		return 0;
	}
	if ((func->thistype != ARIKKEI_TYPE_NONE) && !arikkei_type_is_a (thisval->type, func->thistype)) {
		*canconvert = 0;
		return 0;
	}
	compatible = 1;
	*canconvert = 1;
	for (i = 0; i < func->nargs; i++) {
		if (arikkei_type_is_a (args[i].type, func->argtypes[i])) continue;
		compatible = 0;
		if (canconvert && !arikkei_value_can_convert (func->argtypes[i], args[i].type)) {
			*canconvert = 0;
			return 0;
		}
	}
	return compatible;
}

unsigned int
arikkei_function_convert_arguments (ArikkeiFunction *func, ArikkeiValue *dst, ArikkeiValue *src)
{
	unsigned int i;
	arikkei_return_val_if_fail (func != NULL, 0);
	arikkei_return_val_if_fail (ARIKKEI_IS_FUNCTION (func), 0);
	for (i = 0; i < func->nargs; i++) {
		if (!arikkei_value_convert (&dst[i], func->argtypes[i], &src[i])) return 0;
	}
	return 1;
}

unsigned int
arikkei_function_invoke (ArikkeiFunction *func, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args, unsigned int checktypes)
{
	arikkei_return_val_if_fail (func != NULL, 0);
	arikkei_return_val_if_fail (ARIKKEI_IS_FUNCTION (func), 0);
	if (checktypes) {
		unsigned int i;
		if (func->thistype != ARIKKEI_TYPE_NONE) {
			arikkei_return_val_if_fail (arikkei_type_is_a (thisval->type, func->thistype), 0);
		}
		arikkei_return_val_if_fail ((func->rettype == ARIKKEI_TYPE_NONE) || (retval != NULL), 0);
		for (i = 0; i < func->nargs; i++) {
			arikkei_return_val_if_fail (arikkei_type_is_a (args[i].type, func->argtypes[i]), 0);
		}
	}
	if (((ArikkeiFunctionClass *) ((ArikkeiObject *) func)->klass)->invoke) {
		return ((ArikkeiFunctionClass *) ((ArikkeiObject *) func)->klass)->invoke (func, thisval, retval, args);
	}
	return 0;
}

unsigned int
arikkei_function_invoke_direct (ArikkeiFunction *func, ArikkeiValue *thisval, ArikkeiValue *retval, ...)
{
	ArikkeiValue *vals;
	va_list ap;
	unsigned int result, i;
	arikkei_return_val_if_fail (func != NULL, 0);
	arikkei_return_val_if_fail (ARIKKEI_IS_FUNCTION (func), 0);
	if (func->thistype != ARIKKEI_TYPE_NONE) {
		arikkei_return_val_if_fail (arikkei_type_is_a (thisval->type, func->thistype), 0);
	}
	arikkei_return_val_if_fail ((func->rettype == ARIKKEI_TYPE_NONE) || (retval != NULL), 0);
	vals = (ArikkeiValue *) malloc (func->nargs * sizeof (ArikkeiValue));
	memset (vals, 0, func->nargs * sizeof (ArikkeiValue));
	va_start (ap, func->nargs);
	for (i = 0; i < func->nargs; i++) {
		vals[i].type = func->argtypes[i];
		switch (func->argtypes[i]) {
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
	result = arikkei_function_invoke (func, thisval, retval, vals, 0);
	free (vals);
	return result;
}


