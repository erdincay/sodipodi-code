#ifndef __ARIKKEI_PROPERTY_H__
#define __ARIKKEI_PROPERTY_H__

/*
 * A named property descriptor in classes
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 * 
 */

#ifdef WIN32
#define inline __inline
#endif

#include <libarikkei/arikkei-value.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _ArikkeiProperty {
	ArikkeiString *key;
	unsigned int type;
	/* Id is for fast indexing in classes */
	unsigned int id : 16;
	/* Mutation types */
	unsigned int is_static : 1;
	unsigned int can_read : 1;
	unsigned int can_write : 1;
	unsigned int is_final : 1;
	/* If true the static value is stored in property itself */
	unsigned int is_value : 1;
	/* Default value, may be the actual value for final properties */
	ArikkeiValue value;
};

void arikkei_property_setup (ArikkeiProperty *prop, const unsigned char *key, unsigned int type, unsigned int id,
	unsigned int is_static, unsigned int can_read, unsigned int can_write, unsigned int is_final, unsigned int is_value,
	unsigned int value_type, void *value);

#ifdef __cplusplus
};
#endif

#endif
