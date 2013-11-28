#ifndef __ARIKKEI_OBJECT_H__
#define __ARIKKEI_OBJECT_H__

/*
 * RGBA display list system for sodipodi
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#define ARIKKEI_TYPE_OBJECT (arikkei_object_get_type ())
#define ARIKKEI_OBJECT(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), ARIKKEI_TYPE_OBJECT, ArikkeiObject))
#define ARIKKEI_IS_OBJECT(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), ARIKKEI_TYPE_OBJECT))

typedef struct _ArikkeiObjectClass ArikkeiObjectClass;

#include <libarikkei/arikkei-types.h>
#include <libarikkei/arikkei-utils.h>
#include <libarikkei/arikkei-value.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/* ArikkeiObject */

ARIKKEI_A16 struct _ArikkeiObject {
	ArikkeiObjectClass *klass;
	/* Whether object is standalone of interface */
	unsigned int is_interface;
	/* We can safely add extra flags field here for subclass use */
	unsigned int flags : 31;
	/* Standalone objects have reference count */
	/* For interfaces it is a distance from owner */
	unsigned int refcount;
};

ARIKKEI_A16 struct _ArikkeiObjectClass {
	ArikkeiClass klass;
};

unsigned int arikkei_object_get_type (void);

/* Dynamic lifecycle */

ArikkeiObject *arikkei_object_new (unsigned int type);
ArikkeiObject *arikkei_object_delete (ArikkeiObject *object);

void arikkei_object_ref (ArikkeiObject *object);
void arikkei_object_unref (ArikkeiObject *object);

/* Automatic lifecycle */

/* Setup initializes refcount to 1 */
void arikkei_object_setup (ArikkeiObject *object, unsigned int type);
/* Does not reference owner */
void arikkei_object_setup_interface (ArikkeiObject *object, ArikkeiObject *owner, unsigned int type);
void arikkei_object_release (ArikkeiObject *object);

/* Does not increase reference count of owner object */
void *arikkei_object_get_interface (ArikkeiObject *object, unsigned int type);

/* Helper for propoerty setup */
void arikkei_object_class_property_setup (ArikkeiObjectClass *klass, unsigned int idx, const unsigned char *key, unsigned int type,
										  unsigned int isstatic, unsigned int canread, unsigned int canwrite, unsigned int isfinal, void *value);
/* Create new function as default value of property */
void arikkei_object_class_method_setup (ArikkeiObjectClass *klass, unsigned int idx, const unsigned char *key,
										unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
										unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *));
void arikkei_object_class_static_method_setup (ArikkeiObjectClass *klass, unsigned int idx, const unsigned char *key,
											   unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
											   unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *));


#ifdef __cplusplus
};
#endif

#endif

