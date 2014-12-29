#define __ARIKKEI_INTERFACE_C__

/*
 * Interface class
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 * 
 */

#include <stdlib.h>

#include "arikkei-utils.h"

#include "arikkei-interface.h"

ArikkeiInterfaceClass *arikkei_register_interface_type (unsigned int *type, unsigned int parent, const unsigned char *name,
							unsigned int class_size, unsigned int implementation_size, unsigned int instance_size,
							void (* class_init) (ArikkeiClass *),
							void (* implementation_init) (ArikkeiInterfaceImplementation *),
							void (* instance_init) (ArikkeiInterfaceImplementation *, void *), void (* instance_finalize) (ArikkeiInterfaceImplementation *, void *))
{
	ArikkeiInterfaceClass *ifklass;
	arikkei_return_val_if_fail (arikkei_type_is_a (parent, ARIKKEI_TYPE_INTERFACE), NULL);
	ifklass = (ArikkeiInterfaceClass *) arikkei_register_type (type, parent, name, class_size, instance_size, class_init, NULL, NULL);
	ifklass->implementation_size = implementation_size;
	ifklass->implementation_init = implementation_init;
	ifklass->instance_init = instance_init;
	ifklass->instance_finalize = instance_finalize;
	return ifklass;
}

void *
arikkei_interface_get_instance (ArikkeiInterfaceImplementation *implementation, void *containing_instance)
{
	return (char *) containing_instance + implementation->instance_offset;
}

ArikkeiClass *
arikkei_interface_get_containing_class (ArikkeiInterfaceImplementation *implementation)
{
	return (ArikkeiClass *) ((char *) implementation - implementation->class_offset);
}

void *
arikkei_interface_get_containing_instance (ArikkeiInterfaceImplementation *implementation, void *instance)
{
	return (char *) instance - implementation->instance_offset;
}

