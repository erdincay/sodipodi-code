#define __ARIKKEI_VALUE_C__

/*
 * An universal container encapsulating value and class
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 * 
 */

#include <string.h>

#include "arikkei-reference.h"
#include "arikkei-string.h"
#include "arikkei-object.h"

#include "arikkei-value.h"

void
arikkei_value_set (ArikkeiValue *dst, unsigned int type, void *val)
{
	int ivalue;
	switch (type) {
	case ARIKKEI_TYPE_NONE:
		arikkei_value_clear (dst);
		break;
	case ARIKKEI_TYPE_BOOLEAN:
		arikkei_value_set_boolean (dst, ARIKKEI_POINTER_TO_INT(val));
		break;
	case ARIKKEI_TYPE_INT8:
		ivalue = ARIKKEI_POINTER_TO_INT(val);
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = (i8) ivalue;
		break;
	case ARIKKEI_TYPE_UINT8:
		ivalue = ARIKKEI_POINTER_TO_INT(val);
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = (u8) ivalue;
		break;
	case ARIKKEI_TYPE_INT16:
		ivalue = ARIKKEI_POINTER_TO_INT(val);
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = (i16) ivalue;
		break;
	case ARIKKEI_TYPE_UINT16:
		ivalue = ARIKKEI_POINTER_TO_INT(val);
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = (u16) ivalue;
		break;
	case ARIKKEI_TYPE_INT32:
		arikkei_value_set_i32 (dst, (i32) ARIKKEI_POINTER_TO_INT(val));
		break;
	case ARIKKEI_TYPE_UINT32:
		arikkei_value_set_u32 (dst, (u32) ARIKKEI_POINTER_TO_INT(val));
		break;
	case ARIKKEI_TYPE_INT64:
		/* fixme */
		arikkei_value_set_i64 (dst, (i64) ARIKKEI_POINTER_TO_INT(val));
		break;
	case ARIKKEI_TYPE_UINT64:
		arikkei_value_set_u64 (dst, (u64) ARIKKEI_POINTER_TO_INT(val));
		break;
	case ARIKKEI_TYPE_FLOAT:
		arikkei_value_set_f32 (dst, *((f32 *) val));
		break;
	case ARIKKEI_TYPE_DOUBLE:
		arikkei_value_set_f64 (dst, *((f64 *) val));
		break;
	case ARIKKEI_TYPE_POINTER:
	case ARIKKEI_TYPE_STRUCT:
	case ARIKKEI_TYPE_CLASS:
	case ARIKKEI_TYPE_INTERFACE:
		arikkei_value_clear (dst);
		dst->type = type;
		dst->pvalue = val;
		break;
	case ARIKKEI_TYPE_STRING:
		arikkei_value_set_string (dst, (ArikkeiString *) val);
		break;
	default:
		/* fixme: Make object primitive type? reference? */
		if (arikkei_type_is_a (type, ARIKKEI_TYPE_REFERENCE)) {
			arikkei_value_set_reference (dst, type, (ArikkeiReference *) val);
		} else if (arikkei_type_is_a (type, ARIKKEI_TYPE_STRUCT)) {
			arikkei_value_clear (dst);
			dst->type = type;
			dst->pvalue = val;
			break;
		} else {
			type = arikkei_type_get_parent_primitive (type);
			arikkei_value_set (dst, type, val);
		}
		break;
	}
}

void *
arikkei_value_get_instance (ArikkeiValue *value)
{
	switch (value->type) {
	case ARIKKEI_TYPE_NONE:
	case ARIKKEI_TYPE_ANY:
		break;
	case ARIKKEI_TYPE_INT8:
	case ARIKKEI_TYPE_UINT8:
	case ARIKKEI_TYPE_INT16:
	case ARIKKEI_TYPE_UINT16:
	case ARIKKEI_TYPE_INT32:
	case ARIKKEI_TYPE_UINT32:
		return &value->ivalue;
	case ARIKKEI_TYPE_INT64:
	case ARIKKEI_TYPE_UINT64:
		return &value->lvalue;
	case ARIKKEI_TYPE_FLOAT:
		return &value->fvalue;
	case ARIKKEI_TYPE_DOUBLE:
		return &value->dvalue;
	default:
		return value->pvalue;
	}
	return NULL;
}

void
arikkei_value_set_from_instance (ArikkeiValue *value, unsigned int type, const void *instance)
{
	arikkei_value_clear (value);
	value->type = type;
	switch (type) {
	case ARIKKEI_TYPE_NONE:
	case ARIKKEI_TYPE_ANY:
		break;
	case ARIKKEI_TYPE_INT8:
	case ARIKKEI_TYPE_UINT8:
		value->ivalue = *((i8 *) instance);
		break;
	case ARIKKEI_TYPE_INT16:
	case ARIKKEI_TYPE_UINT16:
		value->ivalue = *((i16 *) instance);
		break;
	case ARIKKEI_TYPE_INT32:
	case ARIKKEI_TYPE_UINT32:
		value->ivalue = *((i32 *) instance);
		break;
	case ARIKKEI_TYPE_INT64:
	case ARIKKEI_TYPE_UINT64:
		value->lvalue = *((i64 *) instance);
		break;
	case ARIKKEI_TYPE_FLOAT:
		value->fvalue = *((float *) instance);
		break;
	case ARIKKEI_TYPE_DOUBLE:
		value->dvalue = *((double *) instance);
		break;
	default:
		if (arikkei_type_is_a (type, ARIKKEI_TYPE_REFERENCE)) {
			value->reference = (ArikkeiReference *) instance;
			arikkei_reference_ref ((ArikkeiReferenceClass *) arikkei_type_get_class (type), value->reference);
			break;
		} else if (arikkei_type_is_a (type, ARIKKEI_TYPE_STRING)) {
			value->string = (ArikkeiString *) instance;
			arikkei_string_ref (value->string);
			break;
		} else {
			value->pvalue = (void *) instance;
		}
	}
}

