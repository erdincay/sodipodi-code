#ifndef __ARIKKEI_VALUE_H__
#define __ARIKKEI_VALUE_H__

/*
 * An universal container encapsulating value and class
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 * 
 */

#ifdef _WIN32
#define ARIKKEI_INLINE __inline
#else
#define ARIKKEI_INLINE inline
#endif

#include <libarikkei/arikkei-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Struct with pvalue NULL functions as NULL value */

struct _ArikkeiValue {
	unsigned int type;
	union {
		unsigned int bvalue;
		i32 ivalue;
		u32 uvalue;
		i64 lvalue;
		f32 fvalue;
		f64 dvalue;
		void *pvalue;
		ArikkeiReference *reference;
		ArikkeiString *string;
		ArikkeiObject *object;
	};
};

void arikkei_value_clear (ArikkeiValue *value);

#define ARIKKEI_VALUE_IS_NULL(v) (((v)->type == ARIKKEI_TYPE_STRUCT) && ((v)->pvalue == NULL))

ARIKKEI_INLINE void
arikkei_value_set_boolean (ArikkeiValue *value, unsigned int val)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_BOOLEAN;
	value->bvalue = val;
}

ARIKKEI_INLINE void
arikkei_value_set_i32 (ArikkeiValue *value, i32 val)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_INT32;
	value->ivalue = val;
}

ARIKKEI_INLINE void
arikkei_value_set_u32 (ArikkeiValue *value, u32 val)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_UINT32;
	value->ivalue = (i32) val;
}

ARIKKEI_INLINE void
arikkei_value_set_i64 (ArikkeiValue *value, i64 val)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_INT64;
	value->lvalue = val;
}

ARIKKEI_INLINE void
arikkei_value_set_u64 (ArikkeiValue *value, u64 val)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_UINT64;
	value->lvalue = (i64) val;
}

ARIKKEI_INLINE void
arikkei_value_set_f32 (ArikkeiValue *value, f32 val)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_FLOAT;
	value->fvalue = val;
}

ARIKKEI_INLINE void
arikkei_value_set_f64 (ArikkeiValue *value, f64 val)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_DOUBLE;
	value->dvalue = val;
}

ARIKKEI_INLINE void
arikkei_value_set_pointer (ArikkeiValue *value, const void *val)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_POINTER;
	value->pvalue = (void *) val;
}

ARIKKEI_INLINE void
arikkei_value_set_class (ArikkeiValue *value, ArikkeiClass *val)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_CLASS;
	value->pvalue = val;
}

void arikkei_value_set_reference (ArikkeiValue *value, unsigned int type, ArikkeiReference *ref);
void arikkei_value_set_string (ArikkeiValue *value, ArikkeiString *str);
void arikkei_value_set_object (ArikkeiValue *value, ArikkeiObject *obj);

ARIKKEI_INLINE void
arikkei_value_transfer_reference (ArikkeiValue *value, unsigned int type, ArikkeiReference *ref)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = type;
	value->reference = ref;
}

ARIKKEI_INLINE void
arikkei_value_transfer_string (ArikkeiValue *value, ArikkeiString *str)
{
	if (value->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (value);
	value->type = ARIKKEI_TYPE_STRING;
	value->string = str;
}

void arikkei_value_copy_indirect (ArikkeiValue *dst, const ArikkeiValue *src);

ARIKKEI_INLINE void
arikkei_value_copy (ArikkeiValue *dst, const ArikkeiValue *src)
{
	if (dst == src) return;
	if (dst->type >= ARIKKEI_TYPE_REFERENCE) arikkei_value_clear (dst);
	if (src->type < ARIKKEI_TYPE_REFERENCE) {
		*dst = *src;
	} else {
		arikkei_value_copy_indirect (dst, src);
	}
}

/* Everything but float/double are by value */
void arikkei_value_set (ArikkeiValue *dst, unsigned int type, void *val);

unsigned int arikkei_value_can_convert (unsigned int to, const ArikkeiValue *from);
unsigned int arikkei_value_convert (ArikkeiValue *dst, unsigned int type, const ArikkeiValue *from);

/* Return memory address of value or dereference */
void *arikkei_value_get_instance (ArikkeiValue *value);
void arikkei_value_set_from_instance (ArikkeiValue *value, unsigned int type, const void *instance);

#ifdef __cplusplus
};
#endif

#endif
