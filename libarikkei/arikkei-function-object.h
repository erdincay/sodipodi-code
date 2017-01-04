#ifndef __ARIKKEI_FUNCTION_OBJECT_H__
#define __ARIKKEI_FUNCTION_OBJECT_H__

/*
 * Encapsulated callback object
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 * 
 */

#define ARIKKEI_TYPE_FUNCTION_OBJECT az_function_object_get_type ()
#define ARIKKEI_FUNCTION_OBJECT AZ_FUNCTION_OBJECT
#define ARIKKEI_IS_FUNCTION_OBJECT AZ_IS_FUNCTION_OBJECT

typedef struct _AZFunctionObject ArikkeiFunctionObject;
typedef struct _AZFunctionObjectClass ArikkeiFunctionObjectClass;

#include <az/function-object.h>

#ifdef __cplusplus
extern "C" {
#endif

#define arikkei_function_object_new az_function_object_new
#define arikkei_function_object_invoke az_function_object_invoke

#ifdef __cplusplus
};
#endif

#endif

