#ifndef __ARIKKEI_FUNCTION_H__
#define __ARIKKEI_FUNCTION_H__

/*
 * Generic function interface
 */

#define ARIKKEI_TYPE_FUNCTION (arikkei_function_get_type ())
/* This casts containing instance to unspecified type */
#define ARIKKEI_FUNCTION(o) ((ArikkeiFunction *) (o))

typedef struct _ArikkeiFunction ArikkeiFunction;
typedef struct _ArikkeiFunctionClass ArikkeiFunctionClass;
typedef struct _ArikkeiFunctionImplementation ArikkeiFunctionImplementation;
typedef struct _ArikkeiFunctionInstance ArikkeiFunctionInstance;

#include <libarikkei/arikkei-interface.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _ArikkeiFunctionInstance {
	/* This object type */
	unsigned int thistype;
	/* Arguments */
	unsigned int nargs;
	unsigned int *argtypes;
	/* Return type */
	unsigned int rettype;
};

struct _ArikkeiFunctionClass {
	ArikkeiInterfaceClass parent_class;
};

struct _ArikkeiFunctionImplementation {
	ArikkeiInterfaceImplementation iface;
	/* All values can be NULL */
	unsigned int (* invoke) (ArikkeiFunctionImplementation *implementation, ArikkeiFunctionInstance *instance, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args);
};

unsigned int arikkei_function_get_type (void);

/* Return true if arguments can be submitted as is */
unsigned int arikkei_function_check_arguments (ArikkeiFunctionImplementation *implementation, ArikkeiFunctionInstance *func, ArikkeiValue *thisval, ArikkeiValue *args, unsigned int nargs, unsigned int *canconvert);
unsigned int arikkei_function_convert_arguments (ArikkeiFunctionImplementation *implementation, ArikkeiFunctionInstance *func, ArikkeiValue *dst, ArikkeiValue *src);

unsigned int arikkei_function_invoke (ArikkeiFunctionImplementation *implementation, ArikkeiFunctionInstance *instance, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args, unsigned int checktypes);
unsigned int arikkei_function_invoke_direct (ArikkeiFunctionImplementation *implementation, ArikkeiFunctionInstance *instance, ArikkeiValue *thisval, ArikkeiValue *retval, ...);

/* Helper */
unsigned int arikkei_function_invoke_by_type_instance (unsigned int type, void *instance, ArikkeiValue *thisval, ArikkeiValue *retval, ArikkeiValue *args, unsigned int checktypes);

#ifdef __cplusplus
};
#endif

#endif

