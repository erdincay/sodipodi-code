#ifndef __ARIKKEI_OBJECT_H__
#define __ARIKKEI_OBJECT_H__

/*
 * Basic type and object system
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

typedef struct _AZObject ArikkeiObject;
typedef struct _AZObjectClass ArikkeiObjectClass;

#define ARIKKEI_TYPE_OBJECT AZ_TYPE_OBJECT
#define ARIKKEI_OBJECT AZ_OBJECT
#define ARIKKEI_IS_OBJECT AZ_IS_OBJECT

#include <libarikkei/arikkei-types.h>
#include <libarikkei/arikkei-value.h>

#include <az/object.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ArikkeiObject */

/* Indicates that object is set up and not yet disposed */
#define ARIKKEI_OBJECT_ALIVE AZ_OBJECT_IS_ALIVE

/*unsigned int arikkei_object_get_type (void);*/

#define arikkei_object_ref(obj) az_reference_ref (&((AZObject *) (obj))->klass->reference_klass, (AZReference *) (obj))
#define arikkei_object_unref(obj) az_reference_unref (&((AZObject *) (obj))->klass->reference_klass, (AZReference *) (obj))

#define arikkei_object_shutdown az_object_shutdown

#define arikkei_object_new az_object_new

#define ARIKKEI_CHECK_INSTANCE_CAST AZ_CHECK_INSTANCE_CAST
#define ARIKKEI_CHECK_INSTANCE_TYPE AZ_CHECK_INSTANCE_TYPE

#define ARIKKEI_OBJECT_GET_CLASS(ip) (ARIKKEI_OBJECT(ip)->klass)
#define ARIKKEI_OBJECT_GET_PARENT_CLASS(ip) (ARIKKEI_OBJECT(ip)->klass->parent)

#define arikkei_object_is_a az_object_is_a
#define arikkei_object_implements_a az_object_implements_a

#define arikkei_object_get_interface az_object_get_interface
#define arikkei_value_set_object az_value_set_object

#ifdef __cplusplus
};
#endif

#endif

