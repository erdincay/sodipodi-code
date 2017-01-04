#ifndef __ARIKKEI_FUNCTION_H__
#define __ARIKKEI_FUNCTION_H__

/*
 * Generic function interface
 */

#define ARIKKEI_TYPE_FUNCTION az_function_get_type ()
/* This casts containing instance to unspecified type */
#define ARIKKEI_FUNCTION(o) ((ArikkeiFunction *) (o))

typedef struct _ArikkeiFunction ArikkeiFunction;
typedef struct _AZFunctionClass ArikkeiFunctionClass;
typedef struct _AZFunctionImplementation ArikkeiFunctionImplementation;
typedef struct _AZFunctionInstance ArikkeiFunctionInstance;

#include <libarikkei/arikkei-interface.h>

#include <az/function.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Return true if arguments can be submitted as is */
#define arikkei_function_check_arguments az_function_check_arguments
#define arikkei_function_convert_arguments az_function_convert_arguments

#define arikkei_function_invoke az_function_invoke
#define arikkei_function_invoke_direct az_function_invoke_direct

/* Helper */
#define arikkei_function_invoke_by_type_instance az_function_invoke_by_type_instance

#ifdef __cplusplus
};
#endif

#endif

