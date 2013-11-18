#define __ARIKKEI_VALUE_C__

/*
 * Basic cross-platform functionality
 */

#include "arikkei-string.h"
#include "arikkei-object.h"

#include "arikkei-value.h"

void
arikkei_value_clear (ArikkeiValue *value)
{
	if (value->type == ARIKKEI_TYPE_STRING) {
		if (value->string) arikkei_string_unref (value->string);
	} else if (arikkei_type_is_a (value->type, ARIKKEI_TYPE_OBJECT)) {
		arikkei_object_unref (value->object);
	}
	value->type = ARIKKEI_TYPE_NONE;
}

void
arikkei_value_set_string (ArikkeiValue *value, ArikkeiString *str)
{
	if (value->type >= ARIKKEI_TYPE_STRING) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_STRING;
	value->string = str;
	if (str) arikkei_string_ref (str);
}

void
arikkei_value_set_object (ArikkeiValue *value, ArikkeiObject *obj)
{
	if (value->type >= ARIKKEI_TYPE_STRING) arikkei_value_clear (value);
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
		arikkei_value_set_boolean (dst, *((unsigned int *) val));
		break;
	case ARIKKEI_TYPE_INT8:
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = *((i8 *) val);
		break;
	case ARIKKEI_TYPE_UINT8:
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = *((u8 *) val);
		break;
	case ARIKKEI_TYPE_INT16:
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = *((i16 *) val);
		break;
	case ARIKKEI_TYPE_UINT16:
		arikkei_value_clear (dst);
		dst->type = type;
		dst->ivalue = *((u16 *) val);
		break;
	case ARIKKEI_TYPE_INT32:
		arikkei_value_set_i32 (dst, *((i32 *) val));
		break;
	case ARIKKEI_TYPE_UINT32:
		arikkei_value_set_u32 (dst, *((i32 *) val));
		break;
	case ARIKKEI_TYPE_INT64:
		arikkei_value_set_i64 (dst, *((i64 *) val));
		break;
	case ARIKKEI_TYPE_UINT64:
		arikkei_value_set_u64 (dst, *((u64 *) val));
		break;
	case ARIKKEI_TYPE_FLOAT:
		arikkei_value_set_f32 (dst, *((f32 *) val));
		break;
	case ARIKKEI_TYPE_DOUBLE:
		arikkei_value_set_f64 (dst, *((f64 *) val));
		break;
	case ARIKKEI_TYPE_POINTER:
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
		if (arikkei_type_is_a (type, ARIKKEI_TYPE_OBJECT)) {
			arikkei_value_set_object (dst, (ArikkeiObject *) val);
		}
		break;
	}
}

