#ifndef __ARIKKEI_REFERENCE_H__
#define __ARIKKEI_REFERENCE_H__

/*
 * Base class for all reference-counted objects
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#define ARIKKEI_REFERENCE(i) ((ArikkeiReference *) (i))

typedef struct _AZReferenceClass ArikkeiReferenceClass;

#include <az/reference.h>

#include <libarikkei/arikkei-types.h>

#define arikkei_reference_ref az_reference_ref
#define arikkei_reference_unref az_reference_unref
#define arikkei_reference_dispose az_reference_dispose

#endif

