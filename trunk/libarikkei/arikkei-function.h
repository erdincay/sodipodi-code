#ifndef __ARIKKEI_FUNCTION_H__
#define __ARIKKEI_FUNCTION_H__

/*
 * Generic object system
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#define ARIKKEI_TYPE_FUNCTION (arikkei_function_get_type ())
#define ARIKKEI_FUNCTION(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), ARIKKEI_TYPE_FUNCTION, ArikkeiFunction))
#define ARIKKEI_IS_FUNCTION(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), ARIKKEI_TYPE_FUNCTION))

typedef struct _ArikkeiFunction ArikkeiFunction;
typedef struct _ArikkeiFunctionClass ArikkeiFunctionClass;

#include <libarikkei/arikkei-object.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _ArikkeiFunction {
	ArikkeiObject object;
	/* This object type */
	unsigned int thistype;
	/* Arguments */
	unsigned int nargs;
	unsigned int *argtypes;
	/* Return type */
	unsigned int rettype;
	/* Function pointer */
	/* Default implementation */
	unsigned int (* call) (ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args);
};

struct _ArikkeiFunctionClass {
	ArikkeiObjectClass parent_class;
	/* All values can be NULL */
	unsigned int (* invoke) (ArikkeiFunction *func, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args);
};

unsigned int arikkei_function_get_type (void);

ArikkeiFunction *arikkei_function_new (unsigned int thistype, unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
									   unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *));

/* Return true if arguments can be submitted as is */
unsigned int arikkei_function_check_arguments (ArikkeiFunction *func, ArikkeiValue *thisval, ArikkeiValue *args, unsigned int *canconvert);
unsigned int arikkei_function_convert_arguments (ArikkeiFunction *func, ArikkeiValue *dst, ArikkeiValue *src);

unsigned int arikkei_function_invoke (ArikkeiFunction *func, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args, unsigned int checktypes);
unsigned int arikkei_function_invoke_direct (ArikkeiFunction *func, ArikkeiValue *thisval, ArikkeiValue *retval, ...);

#ifdef __cplusplus
};
#endif

#endif

