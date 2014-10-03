#ifndef __ARIKKEI_INTERFACE_H__
#define __ARIKKEI_INTERFACE_H__

/*
 * Interface class
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 * 
 */

typedef struct _ArikkeiInterfaceClass ArikkeiInterfaceClass;

#include <libarikkei/arikkei-types.h>

/*
 * Interface has three parts
 *   1 Class
 *   2 Class implementation in containing type class
 *   3 Instance in containing type instance
 */

#ifdef __cplusplus
extern "C" {
#endif

struct _ArikkeiInterfaceClass {
	ArikkeiClass klass;
	/* Size of implementation */
	unsigned int implementation_size;
	/* Constructors and destructors */
	void (* implementation_init) (ArikkeiInterfaceImplementation *implementation);
	/* Instance initialization and cleanup method */
	void (* instance_init) (ArikkeiInterfaceImplementation *implementation, void *iface);
	void (* instance_finalize) (ArikkeiInterfaceImplementation *implementation, void *iface);
};

struct _ArikkeiInterfaceImplementation {
	ArikkeiInterfaceClass *klass;
	unsigned int class_offset;
	unsigned int instance_offset;
};

/* Register new interface type */
ArikkeiInterfaceClass *arikkei_register_interface_type (unsigned int *type, unsigned int parent, const unsigned char *name,
							unsigned int class_size, unsigned int implementation_size, unsigned int instance_size,
							void (* class_init) (ArikkeiClass *),
							void (* implementation_init) (ArikkeiInterfaceImplementation *),
							void (* instance_init) (ArikkeiInterfaceImplementation *, void *), void (* instance_finalize) (ArikkeiInterfaceImplementation *, void *));

void *arikkei_interface_get_instance (ArikkeiInterfaceImplementation *implementation, void *containing_instance);
ArikkeiClass *arikkei_interface_get_containing_class (ArikkeiInterfaceImplementation *implementation);
void *arikkei_interface_get_containing_instance (ArikkeiInterfaceImplementation *implementation, void *instance);

#ifdef __cplusplus
};
#endif

#endif

