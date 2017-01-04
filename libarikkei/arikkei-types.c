#define __ARIKKEI_TYPES_C__

/*
 * Basic cross-platform functionality
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "arikkei-strlib.h"
#include "arikkei-interface.h"
#include "arikkei-function.h"
#include "arikkei-object.h"
#include "arikkei-property.h"
#include "arikkei-reference.h"
#include "arikkei-string.h"
#include "arikkei-value.h"
#include "arikkei-utils.h"
#include "arikkei-function-object.h"

#include "arikkei-types.h"

#if 0
static unsigned int classes_size = 0;
#endif

#define classes az_classes
#define nclasses az_num_classes

unsigned int
arikkei_type_get_parent_primitive (unsigned int type)
{
	ArikkeiClass *klass;
	arikkei_return_val_if_fail (type < nclasses, 0);
	if (type < ARIKKEI_TYPE_NUM_PRIMITIVES) return type;
	klass = classes[type]->parent;
	while (klass) {
		if (klass->implementation.type < ARIKKEI_TYPE_NUM_PRIMITIVES) return klass->implementation.type;
		klass = klass->parent;
	}
	return 0;
}

