#ifndef __ARIKKEI_FUNCTION_OBJECT_H__
#define __ARIKKEI_FUNCTION_OBJECT_H__

/*
 * Encapsulated callback object
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 * 
 */

#define ARIKKEI_TYPE_FUNCTION_OBJECT (arikkei_function_object_get_type ())
#define ARIKKEI_FUNCTION_OBJECT(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), ARIKKEI_TYPE_FUNCTION_OBJECT, ArikkeiFunctionObject))
#define ARIKKEI_IS_FUNCTION_OBJECT(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), ARIKKEI_TYPE_FUNCTION_OBJECT))

typedef struct _ArikkeiFunctionObject ArikkeiFunctionObject;
typedef struct _ArikkeiFunctionObjectClass ArikkeiFunctionObjectClass;

#include <libarikkei/arikkei-function.h>

#include <libarikkei/arikkei-object.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ArikkeiActiveObject */

struct _ArikkeiFunctionObject {
	ArikkeiObject object;
	ArikkeiFunctionInstance function;
	unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *);
};

struct _ArikkeiFunctionObjectClass {
	ArikkeiObjectClass parent_class;
	ArikkeiFunctionImplementation function;
};

unsigned int arikkei_function_object_get_type (void);

ArikkeiFunctionObject *arikkei_function_object_new (unsigned int thistype, unsigned int rettype, unsigned int nargs, const unsigned int argtypes[], unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *));

unsigned int arikkei_function_object_invoke (ArikkeiFunctionObject *fobj, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args, unsigned int checktypes);

#ifdef __cplusplus
};
#endif

#endif

