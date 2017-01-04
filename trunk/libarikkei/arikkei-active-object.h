#ifndef __ARIKKEI_ACTIVE_OBJECT_H__
#define __ARIKKEI_ACTIVE_OBJECT_H__

/*
 * Generic object system
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#define ARIKKEI_TYPE_ACTIVE_OBJECT AZ_TYPE_ACTIVE_OBJECT
#define ARIKKEI_ACTIVE_OBJECT AZ_ACTIVE_OBJECT
#define ARIKKEI_IS_ACTIVE_OBJECT AZ_IS_ACTIVE_OBJECT

typedef struct _AZActiveObject ArikkeiActiveObject;
typedef struct _AZActiveObjectClass ArikkeiActiveObjectClass;

#include <libarikkei/arikkei-object.h>
#include <libarikkei/arikkei-string.h>
#include <libarikkei/arikkei-value.h>

#include <az/active-object.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ArikkeiActiveObject */

typedef struct _AZObjectAttribute ArikkeiObjectAttribute;
typedef struct _AZObjectAttributeArray ArikkeiObjectAttributeArray;
typedef struct _AZObjectListener ArikkeiObjectListener;
typedef struct _AZObjectCallbackBlock ArikkeiObjectCallbackBlock;
typedef struct _AZObjectEventVector ArikkeiObjectEventVector;

#define arikkei_active_object_get_attribute az_active_object_get_attribute
#define arikkei_active_object_set_attribute az_active_object_set_attribute
#define arikkei_active_object_clear_attribute az_active_object_clear_attribute

#define arikkei_active_object_add_listener az_active_object_add_listener
#define arikkei_active_object_remove_listener_by_data az_active_object_remove_listener_by_data

/* Helpers */
#define arikkei_active_object_set_attribute_i32 az_active_object_set_attribute_i32
#define arikkei_active_object_set_attribute_object az_active_object_set_attribute_object

#ifdef __cplusplus
};
#endif

#endif

