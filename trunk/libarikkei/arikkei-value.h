#ifndef __ARIKKEI_VALUE_H__
#define __ARIKKEI_VALUE_H__

/*
 * An universal container encapsulating value and class
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 * 
 */

#include <az/value.h>

#include <libarikkei/arikkei-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Struct with pvalue NULL functions as NULL value */

#define arikkei_value_clear az_value_clear

#define ARIKKEI_VALUE_IS_NULL(v) (((v)->type == ARIKKEI_TYPE_STRUCT) && ((v)->pvalue == NULL))

#define arikkei_value_set_boolean az_value_set_boolean

#define arikkei_value_set_i32(v,i) az_value_set_int (v, AZ_TYPE_INT32, i)
#define arikkei_value_set_u32(v,u) az_value_set_unsigned_int (v, AZ_TYPE_UINT32, u)

#define arikkei_value_set_i64 az_value_set_i64
#define arikkei_value_set_u64 az_value_set_u64
#define arikkei_value_set_f32 az_value_set_f32
#define arikkei_value_set_f64 az_value_set_f64
#define arikkei_value_set_pointer az_value_set_pointer

#define arikkei_value_set_class az_value_set_class
#define arikkei_value_set_reference az_value_set_reference
#define arikkei_value_set_string az_value_set_string

#define arikkei_value_transfer_reference az_value_transfer_reference
#define arikkei_value_transfer_string az_value_transfer_string

#define arikkei_value_copy az_value_copy

/* Everything but float/double are by value */
#define arikkei_value_set az_value_set

#define arikkei_value_can_convert az_value_can_convert
#define arikkei_value_convert az_value_convert

/* Return memory address of value or dereference */
#define arikkei_value_get_instance az_value_get_instance
#define arikkei_value_set_from_instance az_value_set_from_instance

#ifdef __cplusplus
};
#endif

#endif
