#ifndef __ARIKKEI_OBJECT_H__
#define __ARIKKEI_OBJECT_H__

/*
 * Basic type and object system
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

typedef struct _ArikkeiObject ArikkeiObject;
typedef struct _ArikkeiObjectClass ArikkeiObjectClass;

#define ARIKKEI_TYPE_OBJECT (arikkei_object_get_type ())
#define ARIKKEI_OBJECT(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), ARIKKEI_TYPE_OBJECT, ArikkeiObject))
#define ARIKKEI_IS_OBJECT(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), ARIKKEI_TYPE_OBJECT))

typedef struct _ArikkeiObjectInterface ArikkeiObjectInterface;
typedef struct _ArikkeiObjectInstance ArikkeiObjectInstance;
typedef struct _ArikkeiObjectImplementation ArikkeiObjectImplementation;
typedef struct _ArikkeiObjectInterfaceClass ArikkeiObjectInterfaceClass;

#define ARIKKEI_TYPE_OBJECT_INTERFACE (arikkei_object_interface_get_type ())
/* Test and cast object instance */
#define ARIKKEI_OBJECT_INSTANCE(i) (ARIKKEI_CHECK_INTERFACE_INSTANCE_CAST ((i), ARIKKEI_TYPE_OBJECT_INTERFACE, ArikkeiObjectInstance))
#define ARIKKEI_IS_OBJECT_INSTANCE(i) (ARIKKEI_CHECK_INSTANCE_INTERFACE_TYPE ((i), ARIKKEI_TYPE_OBJECT_INTERFACE))
/* Test and cast containing object */
#define ARIKKEI_OBJECT_INTERFACE(i) (ARIKKEI_CHECK_INTERFACE_CAST ((i), ARIKKEI_TYPE_OBJECT_INTERFACE, ArikkeiObjectInterface))
#define ARIKKEI_IS_OBJECT_INTERFACE(i) (ARIKKEI_CHECK_INTERFACE_TYPE ((i), ARIKKEI_TYPE_OBJECT_INTERFACE))
/* Test and cast interface instance */
#define ARIKKEI_OBJECT_INTERFACE_INSTANCE(i) (ARIKKEI_CHECK_INTERFACE_INSTANCE_CAST ((i), ARIKKEI_TYPE_OBJECT_INTERFACE, ArikkeiObjectInstance))
#define ARIKKEI_IS_OBJECT_INTERFACE_INSTANCE(i) (ARIKKEI_CHECK_INTERFACE_INSTANCE_TYPE ((i), ARIKKEI_TYPE_OBJECT_INTERFACE))

#include <libarikkei/arikkei-types.h>
#include <libarikkei/arikkei-utils.h>
#include <libarikkei/arikkei-value.h>
#include <libarikkei/arikkei-interface.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ArikkeiObject */

/* Indicates that object is set up and not yet disposed */
#define ARIKKEI_OBJECT_ALIVE 1

struct _ArikkeiObject {
	ArikkeiObjectClass *klass;
	/* Standalone objects have reference count */
	unsigned int refcount;
	/* As we want header to be 64 bits we can as well have flags */
	unsigned int flags;
};

struct _ArikkeiObjectClass {
	ArikkeiClass klass;
	/* Inform object that the last references to it is about to be dropped */
	/* Returning false means that reference is transferred to another holder */
	unsigned int (* drop) (ArikkeiObject *object);
	/* Dispose should drop resources and references hold by this object */
	void (* dispose) (ArikkeiObject *object);
};

unsigned int arikkei_object_get_type (void);

/* Lifecycle */
/* Unref calls dispose if object is still alive */
void arikkei_object_ref (ArikkeiObject *object);
void arikkei_object_unref (ArikkeiObject *object);

ArikkeiObject *arikkei_object_new (unsigned int type);

/* Dispose drops a reference as well */
void arikkei_object_dispose (ArikkeiObject *object);

#ifndef ARIKKEI_DISABLE_CAST_CHECKS
#define ARIKKEI_CHECK_INSTANCE_CAST(ip, tc, ct) ((ct *) arikkei_object_check_instance_cast (ip, tc))
#else
#define ARIKKEI_CHECK_INSTANCE_CAST(ip, tc, ct) ((ct *) ip)
#endif

#define ARIKKEI_CHECK_INSTANCE_TYPE(ip, tc) arikkei_object_check_instance_type (ip, tc)

#define ARIKKEI_OBJECT_GET_CLASS(ip) (ARIKKEI_OBJECT(ip)->klass)
#define ARIKKEI_OBJECT_GET_PARENT_CLASS(ip) (ARIKKEI_OBJECT(ip)->klass->parent)

void *arikkei_object_check_instance_cast (void *ip, unsigned int tc);
unsigned int arikkei_object_check_instance_type (void *ip, unsigned int tc);

#define arikkei_object_is_a(object, type) arikkei_object_check_instance_type (object, type)

/* ArikkeiObjectInterface */

struct _ArikkeiObjectInstance {
	unsigned int instance_offset;
	unsigned int class_offset;
};

struct _ArikkeiObjectImplementation {
	ArikkeiInterfaceImplementation implementation;
};

struct _ArikkeiObjectInterfaceClass {
	ArikkeiInterfaceClass klass;
};

unsigned int arikkei_object_interface_get_type (void);

void arikkei_object_instance_ref (ArikkeiObjectInstance *iface);
void arikkei_object_instance_unref (ArikkeiObjectInstance *iface);

#define arikkei_object_instance_get_object(i) ((ArikkeiObject *) ((char *) (i) - ((ArikkeiObjectInstance *) (i))->instance_offset))
#define arikkei_object_instance_get_implementation(i) ((ArikkeiObjectImplementation *) ((char *) arikkei_object_instance_get_object(i)->klass + ((ArikkeiObjectInstance *) (i))->class_offset))

#ifndef ARIKKEI_DISABLE_CAST_CHECKS
#define ARIKKEI_CHECK_INTERFACE_CAST(ip, tc, ct) ((ct *) arikkei_object_check_interface_cast (ip, tc))
#define ARIKKEI_CHECK_INTERFACE_INSTANCE_CAST(ip, tc, ct) ((ct *) arikkei_object_check_interface_instance_cast (ip, tc))
#else
#define ARIKKEI_CHECK_INTERFACE_CAST(ip, tc, ct) ((ct *) (ip))
#define ARIKKEI_CHECK_INTERFACE_INSTANCE_CAST(ip, tc, ct) ((ct *) (ip))
#endif

#define ARIKKEI_CHECK_INTERFACE_TYPE(ip, tc) arikkei_object_check_interface_type (ip, tc)
#define ARIKKEI_CHECK_INTERFACE_INSTANCE_TYPE(ip, tc) arikkei_object_check_interface_instance_type (ip, tc)

void *arikkei_object_check_interface_cast (void *ip, unsigned int tc);
unsigned int arikkei_object_check_interface_type (void *ip, unsigned int tc);
void *arikkei_object_check_interface_instance_cast (void *ip, unsigned int tc);
unsigned int arikkei_object_check_interface_instance_type (void *ip, unsigned int tc);

#ifdef __cplusplus
};
#endif

#endif

