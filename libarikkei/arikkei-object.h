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

#include <libarikkei/arikkei-types.h>
#include <libarikkei/arikkei-utils.h>
#include <libarikkei/arikkei-value.h>

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
unsigned int arikkei_object_implements_a (ArikkeiObject *object, unsigned int type);
void *arikkei_object_get_interface (ArikkeiObject *object, unsigned int type, ArikkeiInterfaceImplementation **impl);

#ifdef __cplusplus
};
#endif

#endif

