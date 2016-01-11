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
arikkei_function_object_invoke_private (ArikkeiFunctionImplementation *implementation, ArikkeiFunctionInstance *instance, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args)
{
	ArikkeiFunctionObject *fobj;
	arikkei_return_val_if_fail (implementation != NULL, 0);
	arikkei_return_val_if_fail (arikkei_type_is_a (implementation->iface.type, ARIKKEI_TYPE_FUNCTION), 0);
	arikkei_return_val_if_fail (instance != NULL, 0);
	fobj = (ArikkeiFunctionObject *) arikkei_get_containing_instance ((ArikkeiInterfaceImplementation *) implementation, instance);
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
	fobj->function.thistype = thistype;
	fobj->function.rettype = rettype;
	if (nargs) {
		fobj->function.nargs = nargs;
		fobj->function.argtypes = (unsigned int *) malloc (nargs * sizeof (unsigned int));
		memcpy (fobj->function.argtypes, argtypes, nargs * sizeof (unsigned int));
	}
	fobj->call = call;
	return fobj;
}

unsigned int
arikkei_function_object_invoke (ArikkeiFunctionObject *fobj, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args, unsigned int checktypes)
{
	ArikkeiFunctionImplementation *fimpl;
	ArikkeiFunctionInstance *instance;
	arikkei_return_val_if_fail (fobj != NULL, 0);
	arikkei_return_val_if_fail (ARIKKEI_IS_FUNCTION_OBJECT (fobj), 0);
	fimpl = (ArikkeiFunctionImplementation *) arikkei_implementation_get_interface (&((ArikkeiClass *) fobj->object.klass)->implementation, ARIKKEI_TYPE_FUNCTION);
	instance = (ArikkeiFunctionInstance *) arikkei_get_instance_from_containing_instance ((ArikkeiInterfaceImplementation *) fimpl, fobj);
	return arikkei_function_invoke (fimpl, instance, thisval, retval, args, checktypes);
}

