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
arikkei_value_copy_indirect (ArikkeiValue *dst, const ArikkeiValue *src)
{
	if (dst == src) return;
	if (dst->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (dst);
	if (src->type < ARIKKEI_TYPE_REFERENCE) {
		*dst = *src;
	} else if (arikkei_type_is_a (src->type, ARIKKEI_TYPE_REFERENCE)) {
		arikkei_value_set_reference (dst, src->type, src->reference);
	} else if (arikkei_type_is_a (src->type, ARIKKEI_TYPE_OBJECT)) {
		arikkei_value_set_object (dst, src->object);
	}
}

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
	case ARIKKEI_TYPE_BOOLEAN:
		return (from >= ARIKKEI_TYPE_BOOLEAN);
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
	unsigned int bvalue;
	int ivalue;
	long long lvalue;
	float fvalue;
	double dvalue;
	switch (type) {
	case ARIKKEI_TYPE_NONE:
		arikkei_value_clear (dst);
		return 1;
	case ARIKKEI_TYPE_ANY:
		arikkei_value_copy (dst, from);
		return 1;
	case ARIKKEI_TYPE_BOOLEAN:
		if ((from->type >= ARIKKEI_TYPE_INT8) && (from->type <= ARIKKEI_TYPE_UINT32)) {
			bvalue = from->ivalue != 0;
		} else if ((from->type >= ARIKKEI_TYPE_INT64) && (from->type <= ARIKKEI_TYPE_UINT64)) {
			bvalue = from->lvalue != 0;
		} else if (from->type == ARIKKEI_TYPE_FLOAT) {
			bvalue = from->fvalue != 0;
		} else if (from->type == ARIKKEI_TYPE_DOUBLE) {
			bvalue = from->dvalue != 0;
		} else {
			ivalue = from->pvalue != NULL;
			break;
		}
		arikkei_value_clear (dst);
		dst->bvalue = bvalue;
		dst->type = type;
		return 1;
	case ARIKKEI_TYPE_INT8:
	case ARIKKEI_TYPE_UINT8:
	case ARIKKEI_TYPE_INT16:
	case ARIKKEI_TYPE_UINT16:
	case ARIKKEI_TYPE_INT32:
	case ARIKKEI_TYPE_UINT32:
		if ((from->type >= ARIKKEI_TYPE_INT8) && (from->type <= ARIKKEI_TYPE_UINT32)) {
			ivalue = from->ivalue;
		} else if ((from->type >= ARIKKEI_TYPE_INT64) && (from->type <= ARIKKEI_TYPE_UINT64)) {
			ivalue = (i32) from->lvalue;
		} else if (from->type == ARIKKEI_TYPE_FLOAT) {
			ivalue = (i32) from->fvalue;
		} else if (from->type == ARIKKEI_TYPE_DOUBLE) {
			ivalue = (i32) from->dvalue;
		} else {
			ivalue = 0;
			break;
		}
		arikkei_value_clear (dst);
		dst->ivalue = ivalue;
		dst->type = type;
		return 1;
	case ARIKKEI_TYPE_INT64:
	case ARIKKEI_TYPE_UINT64:
		if ((from->type >= ARIKKEI_TYPE_INT8) && (from->type <= ARIKKEI_TYPE_UINT32)) {
			lvalue = from->ivalue;
		} else if ((from->type >= ARIKKEI_TYPE_INT64) && (from->type <= ARIKKEI_TYPE_UINT64)) {
			lvalue = (i64) from->lvalue;
		} else if (from->type == ARIKKEI_TYPE_FLOAT) {
			lvalue = (i64) from->fvalue;
		} else if (from->type == ARIKKEI_TYPE_DOUBLE) {
			lvalue = (i64) from->dvalue;
		} else {
			lvalue = 0;
			break;
		}
		arikkei_value_clear (dst);
		dst->lvalue = lvalue;
		dst->type = type;
		return 1;
	case ARIKKEI_TYPE_FLOAT:
		if ((from->type >= ARIKKEI_TYPE_INT8) && (from->type <= ARIKKEI_TYPE_UINT32)) {
			fvalue = (f32) from->ivalue;
		} else if ((from->type >= ARIKKEI_TYPE_INT64) && (from->type <= ARIKKEI_TYPE_UINT64)) {
			fvalue = (f32) from->lvalue;
		} else if (from->type == ARIKKEI_TYPE_FLOAT) {
			fvalue = from->fvalue;
		} else if (from->type == ARIKKEI_TYPE_DOUBLE) {
			fvalue = (f32) from->dvalue;
		} else {
			fvalue = 0;
			break;
		}
		arikkei_value_clear (dst);
		dst->fvalue = fvalue;
		dst->type = type;
		return 1;
	case ARIKKEI_TYPE_DOUBLE:
		if ((from->type >= ARIKKEI_TYPE_INT8) && (from->type <= ARIKKEI_TYPE_UINT32)) {
			dvalue = (f64) from->ivalue;
		} else if ((from->type >= ARIKKEI_TYPE_INT64) && (from->type <= ARIKKEI_TYPE_UINT64)) {
			dvalue = (f64) from->lvalue;
		} else if (from->type == ARIKKEI_TYPE_FLOAT) {
			dvalue = (f64) from->fvalue;
		} else if (from->type == ARIKKEI_TYPE_DOUBLE) {
			dvalue = from->dvalue;
		} else {
			dvalue = 0;
			break;
		}
		arikkei_value_clear (dst);
		dst->dvalue = dvalue;
		dst->type = type;
		return 1;
	case ARIKKEI_TYPE_POINTER:
		if (from->type >= ARIKKEI_TYPE_POINTER) {
			void *pvalue = from->pvalue;
			arikkei_value_clear (dst);
			dst->pvalue = pvalue;
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

