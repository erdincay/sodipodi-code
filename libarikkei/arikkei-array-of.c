#define __ARIKKEI_ARRAY_OF_C__

/*
* Value arrays
*
* Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
*
*/

#include <string.h>
#include <stdio.h>

#include <stdlib.h>

#include <libarikkei/arikkei-array-of.h>

unsigned int
arikkei_array_of_get_type (unsigned int element_type)
{
	static unsigned int num_subtypes = 0;
	static unsigned int *subtypes = NULL;
	if (element_type >= num_subtypes) {
		unsigned int new_size = (element_type + 1 + 255) & 0xffffff00;
		subtypes = realloc (subtypes, num_subtypes * sizeof (unsigned int));
		memset (&subtypes[num_subtypes], 0, (new_size - num_subtypes) * sizeof (unsigned int));
		num_subtypes = new_size;
	}
	if (!subtypes[element_type]) {
		ArikkeiClass *element_class = arikkei_type_get_class (element_type);
		unsigned int len = (unsigned int) strlen ((const char *) element_class->name);
		unsigned char *name = malloc (len + 8);
		sprintf ((char *) name, "ArrayOf%s", element_class->name);
		arikkei_register_type (&subtypes[element_type], ARIKKEI_TYPE_ARRAY, name,
			sizeof (ArikkeiArrayOfClass), sizeof (ArikkeiArrayOf),
			NULL, NULL, NULL);
		free (name);
	}
	return subtypes[element_type];
}

ArikkeiClass *
arikkei_register_array_of_type (unsigned int *array_type, unsigned int parent_type, unsigned int element_type, const unsigned char *name,
	unsigned int class_size, unsigned int instance_size,
	void (*class_init) (ArikkeiClass *), void (*instance_init) (void *), void (*instance_finalize) (void *))
{
	ArikkeiArrayOfClass *klass;
	klass = (ArikkeiArrayOfClass *) malloc (class_size);
	memset (klass, 0, class_size);
	klass->element_type = element_type;
	arikkei_register_class ((ArikkeiClass *) klass, array_type, parent_type, name, class_size, instance_size, class_init, instance_init, instance_finalize);
	return (ArikkeiClass *) klass;
}
