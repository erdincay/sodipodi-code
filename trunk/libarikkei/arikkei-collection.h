#ifndef __ARIKKEI_COLLECTION_H__
#define __ARIKKEI_COLLECTION_H__

/*
 * Pure collection interface
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013-2014
 * 
 */

#define ARIKKEI_TYPE_COLLECTION (az_collection_get_type ())

typedef struct _AZCollectionImplementation ArikkeiCollectionImplementation;
typedef struct _AZCollectionClass ArikkeiCollectionClass;

#include <az/collection.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Returns base type that all elements are guaranteed to be assignable to */
#define arikkei_collection_get_element_type az_collection_get_element_type
#define arikkei_collection_get_size az_collection_get_size
/* Return 0 if unusuccessful */
#define arikkei_collection_get_iterator az_collection_get_iterator
#define arikkei_collection_iterator_next az_collection_iterator_next
/* Value will contain the proper type of given element */
#define arikkei_collection_get_element az_collection_get_element

#ifdef __cplusplus
};
#endif

#endif

