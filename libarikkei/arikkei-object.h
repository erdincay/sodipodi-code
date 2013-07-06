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

typedef struct _ArikkeiObject ArikkeiObject;
typedef struct _ArikkeiObjectClass ArikkeiObjectClass;

#define ARIKKEI_TYPE_ACTIVE_OBJECT (arikkei_active_object_get_type ())
#define ARIKKEI_ACTIVE_OBJECT(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), ARIKKEI_TYPE_ACTIVE_OBJECT, ArikkeiActiveObject))
#define ARIKKEI_IS_ACTIVE_OBJECT(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), ARIKKEI_TYPE_ACTIVE_OBJECT))

typedef struct _ArikkeiActiveObject ArikkeiActiveObject;
typedef struct _ArikkeiActiveObjectClass ArikkeiActiveObjectClass;

#include <libarikkei/arikkei-types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define arikkei_return_if_fail(expr) if (!(expr) && arikkei_emit_fail_warning ((const unsigned char *) __FILE__, __LINE__, (const unsigned char *) "?", (const unsigned char *) #expr)) return
#define arikkei_return_val_if_fail(expr,val) if (!(expr) && arikkei_emit_fail_warning ((const unsigned char *) __FILE__, __LINE__, (const unsigned char *) "?", (const unsigned char *) #expr)) return (val)

unsigned int arikkei_emit_fail_warning (const unsigned char *file, unsigned int line, const unsigned char *method, const unsigned char *expr);

#ifndef ARIKKEI_DISABLE_CAST_CHECKS
#define ARIKKEI_CHECK_INSTANCE_CAST(ip, tc, ct) ((ct *) arikkei_object_check_instance_cast (ip, tc))
#else
#define ARIKKEI_CHECK_INSTANCE_CAST(ip, tc, ct) ((ct *) ip)
#endif

#define ARIKKEI_CHECK_INSTANCE_TYPE(ip, tc) arikkei_object_check_instance_type (ip, tc)
#define ARIKKEI_OBJECT_GET_CLASS(ip) (((ArikkeiObject *) ip)->klass)

unsigned int arikkei_type_is_a (unsigned int type, unsigned int test);

void *arikkei_object_check_instance_cast (void *ip, unsigned int tc);
unsigned int arikkei_object_check_instance_type (void *ip, unsigned int tc);

unsigned int arikkei_object_register_type (unsigned int parent,
				      const unsigned char *name,
				      unsigned int csize,
				      unsigned int isize,
				      void (* cinit) (ArikkeiObjectClass *),
				      void (* iinit) (ArikkeiObject *));

/* ArikkeiObject */

ARIKKEI_A16 struct _ArikkeiObject {
	ArikkeiObjectClass *klass;
	unsigned int refcount;
};

ARIKKEI_A16 struct _ArikkeiObjectClass {
	unsigned int type;
	ArikkeiObjectClass *parent;

	unsigned char *name;
	unsigned int csize;
	unsigned int isize;
	void (* cinit) (ArikkeiObjectClass *);
	void (* iinit) (ArikkeiObject *);

	void (* finalize) (ArikkeiObject *object);
};

unsigned int arikkei_object_get_type (void);

/* Dynamic lifecycle */

ArikkeiObject *arikkei_object_new (unsigned int type);
ArikkeiObject *arikkei_object_delete (ArikkeiObject *object);

ArikkeiObject *arikkei_object_ref (ArikkeiObject *object);
ArikkeiObject *arikkei_object_unref (ArikkeiObject *object);

/* Automatic lifecycle */

ArikkeiObject *arikkei_object_setup (ArikkeiObject *object, unsigned int type);
ArikkeiObject *arikkei_object_release (ArikkeiObject *object);

/* ArikkeiActiveObject */

typedef struct _ArikkeiObjectListener ArikkeiObjectListener;
typedef struct _ArikkeiObjectCallbackBlock ArikkeiObjectCallbackBlock;
typedef struct _ArikkeiObjectEventVector ArikkeiObjectEventVector;

struct _ArikkeiObjectEventVector {
	void (* dispose) (ArikkeiObject *object, void *data);
};

struct _ArikkeiObjectListener {
	const ArikkeiObjectEventVector *vector;
	unsigned int size;
	void *data;
};

struct _ArikkeiObjectCallbackBlock {
	unsigned int size;
	unsigned int length;
	ArikkeiObjectListener listeners[1];
};

struct _ArikkeiActiveObject {
	ArikkeiObject object;
	ArikkeiObjectCallbackBlock *callbacks;
};

struct _ArikkeiActiveObjectClass {
	ArikkeiObjectClass parent_class;
};

unsigned int arikkei_active_object_get_type (void);

void arikkei_active_object_add_listener (ArikkeiActiveObject *object, const ArikkeiObjectEventVector *vector, unsigned int size, void *data);
void arikkei_active_object_remove_listener_by_data (ArikkeiActiveObject *object, void *data);

#ifdef __cplusplus
};
#endif

#endif

