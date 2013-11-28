#define __ARIKKEI_VALUE_C__

/*
 * Basic cross-platform functionality
 */

#include <string.h>

#include "arikkei-reference.h"
#include "arikkei-string.h"
#include "arikkei-object.h"

#include "arikkei-value.h"

void
arikkei_value_clear (ArikkeiValue *value)
{
	if (value->type == ARIKKEI_TYPE_STRING) {
		if (value->string) arikkei_string_unref (value->string);
	} else if (arikkei_type_is_a (value->type, ARIKKEI_TYPE_REFERENCE)) {
		if (value->reference) arikkei_reference_unref (arikkei_type_get_class (value->type), value->reference);
	} else if (arikkei_type_is_a (value->type, ARIKKEI_TYPE_OBJECT)) {
		if (value->object) arikkei_object_unref (value->object);
	}
	memset (value, 0, sizeof (ArikkeiValue));
}

void
arikkei_value_set_reference (ArikkeiValue *value, unsigned int type, ArikkeiReference *ref)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = type;
	value->reference = ref;
	if (ref) arikkei_reference_ref (arikkei_type_get_class (type), ref);
}

void
arikkei_value_set_string (ArikkeiValue *value, ArikkeiString *str)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_STRING;
	value->string = str;
	if (str) arikkei_string_ref (str);
}

void
arikkei_value_set_object (ArikkeiValue *value, ArikkeiObject *obj)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = (obj) ? obj->klass->klass.type : ARIKKEI_TYPE_OBJECT;
	value->object = obj;
	if (obj) arikkei_object_ref (obj);
}

void
arikkei_value_set (ArikkeiValue *dst, unsigned int type, void *val)
{
	switch (type) {
	case ARIKKEI_TYPE_NONE:
		arikkei_value_clear (dst);
		break;
	case ARIKKEI_TYPE_BOOLEAN:
		arikkei_value_set_boolean (dst, ARIKKEI_POINTER_TO_INT(val));
		break;
	case ARIKKEI_TYPE_INT8:
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = (i8) ARIKKEI_POINTER_TO_INT(val);
		break;
	case ARIKKEI_TYPE_UINT8:
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = (u8) ARIKKEI_POINTER_TO_INT(val);
		break;
	case ARIKKEI_TYPE_INT16:
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = (i16) ARIKKEI_POINTER_TO_INT(val);
		break;
	case ARIKKEI_TYPE_UINT16:
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = (u16) ARIKKEI_POINTER_TO_INT(val);
		break;
	case ARIKKEI_TYPE_INT32:
		arikkei_value_set_i32 (dst, (i32) ARIKKEI_POINTER_TO_INT(val));
		break;
	case ARIKKEI_TYPE_UINT32:
		arikkei_value_set_u32 (dst, (u32) ARIKKEI_POINTER_TO_INT(val));
		break;
	case ARIKKEI_TYPE_INT64:
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
	case ARIKKEI_TYPE_CLASS:
		arikkei_value_clear (dst);
		dst->type = type;
		dst->pvalue = val;
		break;
	case ARIKKEI_TYPE_STRUCT:
		arikkei_value_clear (dst);
		dst->type = type;
		break;
	case ARIKKEI_TYPE_STRING:
		arikkei_value_set_string (dst, (ArikkeiString *) val);
		break;
	default:
		if (arikkei_type_is_a (type, ARIKKEI_TYPE_REFERENCE)) {
			arikkei_value_set_reference (dst, type, (ArikkeiReference *) val);
		} else if (arikkei_type_is_a (type, ARIKKEI_TYPE_OBJECT)) {
			arikkei_value_set_object (dst, (ArikkeiObject *) val);
		} else if (arikkei_type_is_a (type, ARIKKEI_TYPE_POINTER)) {
			arikkei_value_clear (dst);
			dst->type = type;
			dst->pvalue = val;
		}
		break;
	}
}

unsigned int
arikkei_value_can_convert (unsigned int to, unsigned int from)
{
	if (to == from) return 1;
	switch (to) {
	case ARIKKEI_TYPE_NONE:
		return 1;
	case ARIKKEI_TYPE_ANY:
		return (from == ARIKKEI_TYPE_NONE) ? 0 : 1;
	case ARIKKEI_TYPE_INT8:
	case ARIKKEI_TYPE_UINT8:
	case ARIKKEI_TYPE_INT16:
	case ARIKKEI_TYPE_UINT16:
	case ARIKKEI_TYPE_INT32:
	case ARIKKEI_TYPE_UINT32:
	case ARIKKEI_TYPE_INT64:
	case ARIKKEI_TYPE_UINT64:
	case ARIKKEI_TYPE_FLOAT:
	case ARIKKEI_TYPE_DOUBLE:
		return ((from >= ARIKKEI_TYPE_INT8) && (from <= ARIKKEI_TYPE_DOUBLE));
	case ARIKKEI_TYPE_POINTER:
		return (from >= ARIKKEI_TYPE_POINTER);
	default:
		if (arikkei_type_is_a (from, to)) return 1;
		break;
	}
	return 0;
}

unsigned int
arikkei_value_convert (ArikkeiValue *dst, unsigned int type, const ArikkeiValue *from)
{
	switch (type) {
	case ARIKKEI_TYPE_NONE:
		arikkei_value_clear (dst);
		return 1;
	case ARIKKEI_TYPE_ANY:
		arikkei_value_copy (dst, from);
		return 1;
	case ARIKKEI_TYPE_INT8:
	case ARIKKEI_TYPE_UINT8:
	case ARIKKEI_TYPE_INT16:
	case ARIKKEI_TYPE_UINT16:
	case ARIKKEI_TYPE_INT32:
	case ARIKKEI_TYPE_UINT32:
		if ((from->type >= ARIKKEI_TYPE_INT8) && (from->type <= ARIKKEI_TYPE_UINT32)) {
			arikkei_value_clear (dst);
			dst->ivalue = from->ivalue;
			dst->type = type;
		} else if ((from->type >= ARIKKEI_TYPE_INT64) && (from->type <= ARIKKEI_TYPE_UINT64)) {
			arikkei_value_clear (dst);
			dst->ivalue = (i32) from->lvalue;
			dst->type = type;
		} else if (from->type == ARIKKEI_TYPE_FLOAT) {
			arikkei_value_clear (dst);
			dst->ivalue = (i32) from->fvalue;
			dst->type = type;
		} else if (from->type == ARIKKEI_TYPE_DOUBLE) {
			arikkei_value_clear (dst);
			dst->ivalue = (i32) from->dvalue;
			dst->type = type;
		} else {
			break;
		}
		return 1;
	case ARIKKEI_TYPE_INT64:
	case ARIKKEI_TYPE_UINT64:
		if ((from->type >= ARIKKEI_TYPE_INT8) && (from->type <= ARIKKEI_TYPE_UINT32)) {
			arikkei_value_clear (dst);
			dst->lvalue = from->ivalue;
			dst->type = type;
		} else if ((from->type >= ARIKKEI_TYPE_INT64) && (from->type <= ARIKKEI_TYPE_UINT64)) {
			arikkei_value_clear (dst);
			dst->lvalue = (i64) from->lvalue;
			dst->type = type;
		} else if (from->type == ARIKKEI_TYPE_FLOAT) {
			arikkei_value_clear (dst);
			dst->lvalue = (i64) from->fvalue;
			dst->type = type;
		} else if (from->type == ARIKKEI_TYPE_DOUBLE) {
			arikkei_value_clear (dst);
			dst->lvalue = (i64) from->dvalue;
			dst->type = type;
		} else {
			break;
		}
		return 1;
	case ARIKKEI_TYPE_FLOAT:
		if ((from->type >= ARIKKEI_TYPE_INT8) && (from->type <= ARIKKEI_TYPE_UINT32)) {
			arikkei_value_clear (dst);
			dst->fvalue = (f32) from->ivalue;
			dst->type = type;
		} else if ((from->type >= ARIKKEI_TYPE_INT64) && (from->type <= ARIKKEI_TYPE_UINT64)) {
			arikkei_value_clear (dst);
			dst->fvalue = (f32) from->lvalue;
			dst->type = type;
		} else if (from->type == ARIKKEI_TYPE_FLOAT) {
			arikkei_value_clear (dst);
			dst->fvalue = (f32) from->fvalue;
			dst->type = type;
		} else if (from->type == ARIKKEI_TYPE_DOUBLE) {
			arikkei_value_clear (dst);
			dst->fvalue = (f32) from->dvalue;
			dst->type = type;
		} else {
			break;
		}
		return 1;
	case ARIKKEI_TYPE_DOUBLE:
		if ((from->type >= ARIKKEI_TYPE_INT8) && (from->type <= ARIKKEI_TYPE_UINT32)) {
			arikkei_value_clear (dst);
			dst->dvalue = (f64) from->ivalue;
			dst->type = type;
		} else if ((from->type >= ARIKKEI_TYPE_INT64) && (from->type <= ARIKKEI_TYPE_UINT64)) {
			arikkei_value_clear (dst);
			dst->dvalue = (f64) from->lvalue;
			dst->type = type;
		} else if (from->type == ARIKKEI_TYPE_FLOAT) {
			arikkei_value_clear (dst);
			dst->dvalue = (f64) from->fvalue;
			dst->type = type;
		} else if (from->type == ARIKKEI_TYPE_DOUBLE) {
			arikkei_value_clear (dst);
			dst->dvalue = (f64) from->dvalue;
			dst->type = type;
		} else {
			break;
		}
		return 1;
	case ARIKKEI_TYPE_POINTER:
		if (from->type >= ARIKKEI_TYPE_POINTER) {
			arikkei_value_clear (dst);
			dst->pvalue = from->pvalue;
			dst->type = type;
		} else {
			break;
		}
		return 1;
	default:
		if (arikkei_type_is_a (from->type, type)) {
			arikkei_value_copy (dst, from);
			return 1;
		}
		break;
	}
	return 0;
}
