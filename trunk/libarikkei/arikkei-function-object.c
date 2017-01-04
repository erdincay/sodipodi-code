#define __ARIKKEI_FUNCTION_OBJECT_C__

/*
 * Encapsulated callback object
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 * 
 */

#include <stdlib.h>
#include <string.h>

#include "arikkei-function-object.h"

/* ArikkeiFunctionObject */

static void arikkei_function_object_class_init (ArikkeiFunctionObjectClass *klass);

/* ArikkeiFunction implementation */

static unsigned int arikkei_function_object_invoke_private (ArikkeiFunctionImplementation *implementation, ArikkeiFunctionInstance *instance, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args);

static ArikkeiObjectClass *parent_class;

unsigned int
arikkei_function_object_get_type (void)
{
	static unsigned int type = 0;
	if (!type) {
		arikkei_register_type (&type, ARIKKEI_TYPE_OBJECT,
						(const unsigned char *) "ArikkeiFunctionObject",
						sizeof (ArikkeiFunctionObjectClass),
						sizeof (ArikkeiFunctionObject),
						(void (*) (ArikkeiClass *)) arikkei_function_object_class_init,
						NULL,
						NULL);
	}
	return type;
}

static void
arikkei_function_object_class_init (ArikkeiFunctionObjectClass *klass)
{
	parent_class = (ArikkeiObjectClass *) ((ArikkeiClass *) klass)->parent;
	arikkei_class_set_num_interfaces ((ArikkeiClass *) klass, 1);
	arikkei_interface_implementation_setup ((ArikkeiClass *) klass, 0, ARIKKEI_TYPE_FUNCTION, ARIKKEI_OFFSET(ArikkeiFunctionObjectClass, function), ARIKKEI_OFFSET(ArikkeiFunctionObject, function));
	klass->function.invoke = arikkei_function_object_invoke_private;
}

static unsigned int
arikkei_function_object_invoke_private (ArikkeiFunctionImplementation *impl, ArikkeiFunctionInstance *inst, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args)
{
	ArikkeiFunctionObject *fobj;
	arikkei_return_val_if_fail (impl != NULL, 0);
	arikkei_return_val_if_fail (arikkei_type_is_a (impl->implementation.type, ARIKKEI_TYPE_FUNCTION), 0);
	arikkei_return_val_if_fail (inst != NULL, 0);
	fobj = (ArikkeiFunctionObject *) ((char *) inst - ARIKKEI_OFFSET (ArikkeiFunctionObject, function));
	arikkei_return_val_if_fail (arikkei_object_is_a (fobj, ARIKKEI_TYPE_FUNCTION_OBJECT), 0);
	if (fobj->call) {
		return fobj->call (thisval, retval, args);
	}
	return 0;
}

ArikkeiFunctionObject *
arikkei_function_object_new (unsigned int thistype, unsigned int rettype, unsigned int nargs, const unsigned int argtypes[], unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *))
{
	ArikkeiFunctionObject *fobj;
	fobj = (ArikkeiFunctionObject *) arikkei_object_new (ARIKKEI_TYPE_FUNCTION_OBJECT);
	fobj->function.this_type = thistype;
	fobj->function.ret_type = rettype;
	if (nargs) {
		fobj->function.n_args = nargs;
		fobj->function.arg_types = (unsigned int *) malloc (nargs * sizeof (unsigned int));
		memcpy (fobj->function.arg_types, argtypes, nargs * sizeof (unsigned int));
	}
	fobj->call = call;
	return fobj;
}

unsigned int
arikkei_function_object_invoke (ArikkeiFunctionObject *fobj, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args, unsigned int checktypes)
{
	arikkei_return_val_if_fail (fobj != NULL, 0);
	arikkei_return_val_if_fail (ARIKKEI_IS_FUNCTION_OBJECT (fobj), 0);
	return arikkei_function_invoke (&((ArikkeiFunctionObjectClass *) fobj->object.klass)->function, &fobj->function, thisval, retval, args, checktypes);
}

