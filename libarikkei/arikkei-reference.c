#define __ARIKKEI_REFERENCE_C__

/*
 * Base class for all reference-counted objects
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2013
 * 
 */

#include <stdlib.h>
#include <string.h>

#include "arikkei-reference.h"

void
arikkei_reference_ref (ArikkeiClass *klass, ArikkeiReference *reference)
{
	reference->refcount += 1;
}

void
arikkei_reference_unref (ArikkeiClass *klass, ArikkeiReference *reference)
{
	reference->refcount -= 1;
	if (!reference->refcount) {
		if (klass->instance_finalize) {
			klass->instance_finalize (reference);
		}
		free (reference);
	}
}

