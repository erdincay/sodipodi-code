#ifndef __ARIKKEI_TYPES_H__
#define __ARIKKEI_TYPES_H__

/*
 * Basic cross-platform functionality
 */

#include <az/types.h>

#define ARIKKEI_A16 AZ_ALIGN_16

#ifdef __cplusplus
extern "C" {
#endif

/* Instance is a data unit in memory */
typedef struct _AZInstance ArikkeiInstance;
/* Implementation-dependent aspects of instance */
typedef struct _AZImplementation ArikkeiImplementation;

/*
 * Class describes implementation-independent aspects of instance
 * Class is the default static implementation of a type
 * Thus all non-interface types form a single tree
 */
typedef struct _AZClass ArikkeiClass;

/*
 * To use an instance we need pointer to it and pointer to its implementation
 * From the latter we can get type and pointers to all containing implementations
 */
typedef struct _AZInterfaceClass ArikkeiInterfaceClass;
typedef struct _AZImplementation ArikkeiInterfaceImplementation;

typedef struct _AZValue ArikkeiValue;
typedef struct _AZReference ArikkeiReference;
typedef struct _AZString ArikkeiString;
typedef struct _AZField ArikkeiProperty;
typedef struct _AZObject ArikkeiObject;

/* Typecodes */
#define ARIKKEI_TYPE_NONE AZ_TYPE_NONE
#define ARIKKEI_TYPE_ANY AZ_TYPE_ANY
#define ARIKKEI_TYPE_BOOLEAN AZ_TYPE_BOOLEAN
#define ARIKKEI_TYPE_INT8 AZ_TYPE_INT8
#define ARIKKEI_TYPE_UINT8 AZ_TYPE_UINT8
#define ARIKKEI_TYPE_INT16 AZ_TYPE_INT16
#define ARIKKEI_TYPE_UINT16 AZ_TYPE_UINT16
#define ARIKKEI_TYPE_INT32 AZ_TYPE_INT32
#define ARIKKEI_TYPE_UINT32 AZ_TYPE_UINT32
#define ARIKKEI_TYPE_INT64 AZ_TYPE_INT64
#define ARIKKEI_TYPE_UINT64 AZ_TYPE_UINT64
#define ARIKKEI_TYPE_FLOAT AZ_TYPE_FLOAT
#define ARIKKEI_TYPE_DOUBLE AZ_TYPE_DOUBLE
#define ARIKKEI_TYPE_POINTER AZ_TYPE_POINTER
#define ARIKKEI_TYPE_STRUCT AZ_TYPE_STRUCT
#define ARIKKEI_TYPE_CLASS AZ_TYPE_CLASS
#define ARIKKEI_TYPE_INTERFACE AZ_TYPE_INTERFACE
#define ARIKKEI_TYPE_ARRAY AZ_TYPE_ARRAY
#define ARIKKEI_TYPE_REFERENCE AZ_TYPE_REFERENCE
#define ARIKKEI_TYPE_STRING AZ_TYPE_STRING
#define ARIKKEI_TYPE_VALUE AZ_TYPE_VALUE
#define ARIKKEI_TYPE_NUM_PRIMITIVES AZ_NUM_TYPE_PRIMITIVES

#define _ARIKKEI_CONTAINING_INSTANCE(i,C,N) (C *) ((char *) (i) - ARIKKEI_OFFSET (C, N))

#define ARIKKEI_TYPE_IS_ARITHMETIC(t) (((t) >= ARIKKEI_TYPE_INT8) && ((t) <= ARIKKEI_TYPE_DOUBLE))
#define ARIKKEI_TYPE_IS_INTEGRAL(t) (((t) >= ARIKKEI_TYPE_INT8) && ((t) <= ARIKKEI_TYPE_UINT64))
#define ARIKKEI_TYPE_IS_SIGNED(t) (((t) == ARIKKEI_TYPE_INT8) || ((t) == ARIKKEI_TYPE_INT16) || ((t) == ARIKKEI_TYPE_INT32) || ((t) == ARIKKEI_TYPE_INT64) || ((t) == ARIKKEI_TYPE_FLOAT) || ((t) == ARIKKEI_TYPE_DOUBLE))
#define ARIKKEI_TYPE_IS_UNSIGNED(t) (((t) == ARIKKEI_TYPE_UINT8) || ((t) == ARIKKEI_TYPE_UINT16) || ((t) == ARIKKEI_TYPE_UINT32) || ((t) == ARIKKEI_TYPE_UINT64))
#define ARIKKEI_TYPE_IS_64(t) (((t) == ARIKKEI_TYPE_INT64) || ((t) == ARIKKEI_TYPE_UINT64))

/* Class flags */

#define ARIKKEI_CLASS_IS_ABSTRACT AZ_CLASS_IS_ABSTRACT
#define ARIKKEI_CLASS_ZERO_MEMORY AZ_CLASS_ZERO_MEMORY

/* General purpose methods */

#define arikkei_type_get_class az_type_get_class

#define arikkei_type_is_a az_type_is_a
#define arikkei_type_implements_a az_type_implements_a
/* Returns true if either test is object and type is a subtype or test is interface and object implements it */
#define arikkei_type_is_assignable_to az_type_is_assignable_to
/* Returns most specific builtin parent type for this object */
#define arikkei_type_get_parent_primitive az_type_get_parent_primitive

#define arikkei_class_is_of_type az_class_is_of_type

#define arikkei_instance_new az_instance_new
#define arikkei_instance_new_array az_instance_new_array
#define arikkei_instance_delete az_instance_delete
#define arikkei_instance_delete_array az_instance_delete_array
#define arikkei_instance_setup az_instance_setup
#define arikkei_instance_release az_instance_release

#define arikkei_type_get_name az_type_get_name

#define arikkei_instance_to_string az_instance_to_string

/* Get the topmost interface that implements given type; sub_inst can be NULL if not needed */
#define arikkei_get_interface az_get_interface
#define arikkei_get_interface_from_type az_get_interface_from_type

#define arikkei_class_lookup_property az_class_lookup_property
#define arikkei_instance_set_property az_instance_set_property
#define arikkei_instance_get_property az_instance_get_property
#define arikkei_instance_set_property_by_id az_instance_set_property_by_id
#define arikkei_instance_get_property_by_id az_instance_get_property_by_id

/* For type system use */

void _arikkei_types_init (void);

/* Returns allocated class */
/* Type is guaranteed to be assigned before class constructors are invoked */
#define arikkei_register_type az_register_type

/* Setup helpers, for class implementations */

#define arikkei_class_set_num_interfaces az_class_set_num_interfaces
/* Set up interface inside another interface type */
#define arikkei_implementation_declare az_implementation_declare
/* Set up interface inside standalone type and initializes implementations */
#define arikkei_implementation_setup az_implementation_setup
#define arikkei_interface_implementation_setup az_implementation_setup

#define arikkei_class_set_num_properties az_class_set_num_properties
#define arikkei_class_property_setup az_class_property_setup

#define arikkei_class_method_setup az_class_method_setup
#define arikkei_class_static_method_setup az_class_static_method_setup

/* For special subtype implementations */
/* Register class in type system */
/* Class has to be zeroed and relevant fields of subclasses set up */
/* Type is guaranteed to be assigned before class constructors are invoked */
#define arikkei_register_class az_register_class

#ifdef __cplusplus
};
#endif

#endif
