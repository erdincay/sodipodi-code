#ifndef __ARIKKEI_VALUE_H__
#define __ARIKKEI_VALUE_H__

/*
 * Basic cross-platform functionality
 */

#ifdef WIN32
#define inline __inline
#endif

#include <libarikkei/arikkei-types.h>

#ifdef __cplusplus
extern "C" {
#endif

void arikkei_value_clear (ArikkeiValue *value);

inline void
arikkei_value_set_boolean (ArikkeiValue *value, unsigned int val)
{
	if (value->type >= ARIKKEI_TYPE_STRING) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_BOOLEAN;
	value->bvalue = val;
}

inline void
arikkei_value_set_i32 (ArikkeiValue *value, i32 val)
{
	if (value->type >= ARIKKEI_TYPE_STRING) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_INT32;
	value->ivalue = val;
}

inline void
arikkei_value_set_u32 (ArikkeiValue *value, u32 val)
{
	if (value->type >= ARIKKEI_TYPE_STRING) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_UINT32;
	value->ivalue = (i32) val;
}

inline void
arikkei_value_set_i64 (ArikkeiValue *value, i64 val)
{
	if (value->type >= ARIKKEI_TYPE_STRING) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_INT64;
	value->lvalue = val;
}

inline void
arikkei_value_set_u64 (ArikkeiValue *value, u64 val)
{
	if (value->type >= ARIKKEI_TYPE_STRING) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_UINT64;
	value->lvalue = (i64) val;
}

inline void
arikkei_value_set_f32 (ArikkeiValue *value, f32 val)
{
	if (value->type >= ARIKKEI_TYPE_STRING) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_FLOAT;
	value->fvalue = val;
}

inline void
arikkei_value_set_f64 (ArikkeiValue *value, f64 val)
{
	if (value->type >= ARIKKEI_TYPE_STRING) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_DOUBLE;
	value->dvalue = val;
}

inline void
arikkei_value_set_pointer (ArikkeiValue *value, void *val)
{
	if (value->type >= ARIKKEI_TYPE_STRING) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_POINTER;
	value->pvalue = val;
}

void arikkei_value_set_string (ArikkeiValue *value, ArikkeiString *str);
void arikkei_value_set_object (ArikkeiValue *value, ArikkeiObject *obj);

inline void
arikkei_value_copy (ArikkeiValue *dst, const ArikkeiValue *src)
{
	if (dst->type >= ARIKKEI_TYPE_STRING) arikkei_value_clear (dst);
	if (src->type < ARIKKEI_TYPE_STRING) {
		*dst = *src;
	} else if (src->type == ARIKKEI_TYPE_STRING) {
		arikkei_value_set_string (dst, src->string);
	} else {
		arikkei_value_set_object (dst, src->object);
	}
}

void arikkei_value_set (ArikkeiValue *dst, unsigned int type, void *val);

#ifdef __cplusplus
};
#endif

#endif
