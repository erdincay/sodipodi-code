#ifndef __ARIKKEI_ARRAY_OF_H__
#define __ARIKKEI_ARRAY_OF_H__

/*
 * Value arrays
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 *
 */

typedef struct _ArikkeiArrayOf ArikkeiArrayOf;
typedef struct _ArikkeiArrayOfClass ArikkeiArrayOfClass;

#define ARIKKEI_TYPE_ARRAY_OF(t) (arikkei_array_of_get_type (t))

#include <libarikkei/arikkei-reference.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _ArikkeiArrayOf {
	unsigned int size;
	unsigned char values[1];
};

struct _ArikkeiArrayOfClass {
	ArikkeiClass klass;
	unsigned int element_type;
};

unsigned int arikkei_array_of_get_type (unsigned int element_type);

/* For subtype implementations */
ArikkeiClass *arikkei_register_array_of_type (unsigned int *array_type, unsigned int parent_type, unsigned int element_type, const unsigned char *name,
	unsigned int class_size, unsigned int instance_size,
	void (* class_init) (ArikkeiClass *), void (* instance_init) (void *), void (* instance_finalize) (void *));

#ifdef __cplusplus
};
#endif

#endif

