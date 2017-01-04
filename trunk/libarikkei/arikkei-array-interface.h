#ifndef __ARIKKEI_ARRAY_INTERFACE_H__
#define __ARIKKEI_ARRAY_INTERFACE_H__

/*
 * Linear collection interface
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#define ARIKKEI_TYPE_ARRAY_INTERFACE az_list_get_type ()

typedef struct _AZListImplementation ArikkeiArrayImplementation;
typedef struct _AZListClass ArikkeiArrayInterfaceClass;

#include <libarikkei/arikkei-collection.h>
#include <libarikkei/arikkei-value.h>

#include <az/list.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * An hybrid interface that allows sequential access by index
 * The iterator of an array is always unsigned integer
 */

#define arikkei_array_interface_get_element az_list_get_element

#ifdef __cplusplus
};
#endif

#endif

