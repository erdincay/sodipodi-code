#define __ARIKKEI_PROPERTY_CPP__

/*
 * A named property descriptor in classes
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 * 
 */

#include <stdlib.h>

#include <libarikkei/arikkei-string.h>
#include <libarikkei/arikkei-utils.h>

#include <libarikkei/arikkei-property.h>

void
arikkei_property_setup (ArikkeiProperty *prop, const unsigned char *key, unsigned int type, unsigned int id,
	unsigned int is_static, unsigned int can_read, unsigned int can_write, unsigned int is_final, unsigned int is_value,
	unsigned int value_type, void *value)
{
	arikkei_return_if_fail (prop != NULL);
	arikkei_return_if_fail (key != NULL);
	arikkei_return_if_fail (type != ARIKKEI_TYPE_NONE);
	arikkei_return_if_fail (!(can_write && is_final));
	arikkei_return_if_fail (!is_value || is_static);
	arikkei_return_if_fail ((value_type == ARIKKEI_TYPE_NONE) || (arikkei_type_is_assignable_to (value_type, type)));

	prop->key = arikkei_string_new (key);
	prop->type = type;
	prop->id = id;
	prop->is_static = is_static;
	prop->can_read = can_read;
	prop->can_write = can_write;
	prop->is_final = is_final;
	prop->is_value = is_value;
	if (value_type != ARIKKEI_TYPE_NONE) {
		arikkei_value_set (&prop->value, value_type, value);
	}
}

