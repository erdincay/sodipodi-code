#ifndef __ARIKKEI_ACTIVE_OBJECT_H__
#define __ARIKKEI_ACTIVE_OBJECT_H__

/*
 * Generic object system
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#define ARIKKEI_TYPE_ACTIVE_OBJECT (arikkei_active_object_get_type ())
#define ARIKKEI_ACTIVE_OBJECT(o) (ARIKKEI_CHECK_INSTANCE_CAST ((o), ARIKKEI_TYPE_ACTIVE_OBJECT, ArikkeiActiveObject))
#define ARIKKEI_IS_ACTIVE_OBJECT(o) (ARIKKEI_CHECK_INSTANCE_TYPE ((o), ARIKKEI_TYPE_ACTIVE_OBJECT))

typedef struct _ArikkeiActiveObject ArikkeiActiveObject;
typedef struct _ArikkeiActiveObjectClass ArikkeiActiveObjectClass;

#include <libarikkei/arikkei-object.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ArikkeiActiveObject */

typedef struct _ArikkeiObjectAttribute ArikkeiObjectAttribute;
typedef struct _ArikkeiObjectAttributeArray ArikkeiObjectAttributeArray;
typedef struct _ArikkeiObjectListener ArikkeiObjectListener;
typedef struct _ArikkeiObjectCallbackBlock ArikkeiObjectCallbackBlock;
typedef struct _ArikkeiObjectEventVector ArikkeiObjectEventVector;

struct _ArikkeiObjectAttribute {
	unsigned char *key;
	ArikkeiValue value;
};

struct _ArikkeiObjectAttributeArray {
	unsigned int size;
	unsigned int length;
	ArikkeiObjectAttribute attribs[1];
};

struct _ArikkeiObjectEventVector {
	void (* dispose) (ArikkeiActiveObject *object, void *data);
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
	ArikkeiObjectAttributeArray *attributes;
};

struct _ArikkeiActiveObjectClass {
	ArikkeiObjectClass parent_class;
};

unsigned int arikkei_active_object_get_type (void);

unsigned int arikkei_active_object_get_attribute (ArikkeiActiveObject *aobj, const unsigned char *key, ArikkeiValue *val);
unsigned int arikkei_active_object_set_attribute (ArikkeiActiveObject *aobj, const unsigned char *key, const ArikkeiValue *val);
unsigned int arikkei_active_object_clear_attribute (ArikkeiActiveObject *aobj, const unsigned char *key);

void arikkei_active_object_add_listener (ArikkeiActiveObject *aobj, const ArikkeiObjectEventVector *vector, unsigned int size, void *data);
void arikkei_active_object_remove_listener_by_data (ArikkeiActiveObject *aobj, void *data);

/* Helpers */
unsigned int arikkei_active_object_set_attribute_i32 (ArikkeiActiveObject *aobj, const unsigned char *key, int value);
unsigned int arikkei_active_object_set_attribute_object (ArikkeiActiveObject *aobj, const unsigned char *key, ArikkeiObject *value);

#ifdef __cplusplus
};
#endif

#endif

