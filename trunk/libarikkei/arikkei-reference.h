#ifndef __ARIKKEI_REFERENCE_H__
#define __ARIKKEI_REFERENCE_H__

/*
 * Base class for all reference-counted objects
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#define ARIKKEI_REFERENCE(i) ((ArikkeiReference *) (i))

typedef struct _ArikkeiClass ArikkeiReferenceClass;

#include <libarikkei/arikkei-types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _ArikkeiReference {
	u32 refcount;
};

void arikkei_reference_ref (ArikkeiClass *klass, ArikkeiReference *reference);
void arikkei_reference_unref (ArikkeiClass *klass, ArikkeiReference *reference);

#ifdef __cplusplus
};
#endif

#endif

