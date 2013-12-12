#ifndef __ARIKKEI_TYPES_H__
#define __ARIKKEI_TYPES_H__

/*
 * Basic cross-platform functionality
 */

/* Basic types */

typedef char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32;
typedef unsigned int u32;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ArikkeiValue ArikkeiValue;
typedef struct _ArikkeiReference ArikkeiReference;
typedef struct _ArikkeiString ArikkeiString;
typedef struct _ArikkeiProperty ArikkeiProperty;
typedef struct _ArikkeiClass ArikkeiClass;
typedef struct _ArikkeiCollection ArikkeiCollection;
typedef struct _ArikkeiObject ArikkeiObject;

/* Alignment */

#ifdef WIN32
#define ARIKKEI_A16 __declspec(align(16))
#else
#define ARIKKEI_A16 __attribute__ ((aligned (8)))
#endif

/* Typecodes */

enum {
	ARIKKEI_TYPE_NONE,
	ARIKKEI_TYPE_ANY,
	ARIKKEI_TYPE_BOOLEAN,
	ARIKKEI_TYPE_INT8,
	ARIKKEI_TYPE_UINT8,
	ARIKKEI_TYPE_INT16,
	ARIKKEI_TYPE_UINT16,
	ARIKKEI_TYPE_INT32,
	ARIKKEI_TYPE_UINT32,
	ARIKKEI_TYPE_INT64,
	ARIKKEI_TYPE_UINT64,
	ARIKKEI_TYPE_FLOAT,
	ARIKKEI_TYPE_DOUBLE,
	ARIKKEI_TYPE_POINTER,
	ARIKKEI_TYPE_STRUCT,
	ARIKKEI_TYPE_CLASS,
	ARIKKEI_TYPE_INTERFACE,
	ARIKKEI_TYPE_COLLECTION,
	ARIKKEI_TYPE_REFERENCE,
	ARIKKEI_TYPE_STRING,
	ARIKKEI_TYPE_NUM_PRIMITIVES
};

#define ARIKKEI_INT_TO_POINTER(v) (void *) ((char *) 0 + (v))
#define ARIKKEI_POINTER_TO_INT(p) ((char *) p - (char *) 0)

struct _ArikkeiValue {
	unsigned int type;
	union {
		unsigned int bvalue;
		i32 ivalue;
		i64 lvalue;
		f32 fvalue;
		f64 dvalue;
		void *pvalue;
		ArikkeiReference *reference;
		ArikkeiString *string;
		ArikkeiObject *object;
	};
};

struct _ArikkeiProperty {
	ArikkeiString *key;
	unsigned int type;
	unsigned int id : 16;
	unsigned int is_static : 1;
	unsigned int can_read : 1;
	unsigned int can_write : 1;
	unsigned int is_final : 1;
	ArikkeiValue defval;
};

void arikkei_property_setup (ArikkeiProperty *prop, const unsigned char *key, unsigned int type, unsigned int id, unsigned int isstatic, unsigned int canread, unsigned int canwrite, unsigned int isfinal, void *value);

ARIKKEI_A16 struct _ArikkeiClass {
	unsigned int type;
	ArikkeiClass *parent;
	unsigned int firstproperty;
	unsigned int nproperties;
	ArikkeiProperty *properties;

	const unsigned char *name;
	unsigned int class_size;
	unsigned int instance_size;
	void (* class_init) (ArikkeiClass *klass);
	void (* instance_init) (void *instance);
	void (* instance_finalize) (void *instance);
	unsigned int (* to_string) (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len);
	unsigned int (*get_property) (ArikkeiClass *klass, void *instance, unsigned int idx, ArikkeiValue *val);
	unsigned int (*set_property) (ArikkeiClass *klass, void *instance, unsigned int idx, const ArikkeiValue *val);
	void *(* get_interface) (ArikkeiClass *klass, void *instance, unsigned int type);
};

void arikkei_types_init (void);

/* Creates new class and assigns it a type */
/* Type is guaranteed to be assigned before class constructors are invoked */
void arikkei_register_class (unsigned int *type, ArikkeiClass *klass, unsigned int parent, const unsigned char *name, unsigned int class_size, unsigned int instance_size,
							void (* class_init) (ArikkeiClass *), void (* instance_init) (void *), void (* instance_finalize) (void *));
void arikkei_register_type (unsigned int *type, unsigned int parent, const unsigned char *name, unsigned int class_size, unsigned int instance_size,
							void (* class_init) (ArikkeiClass *), void (* instance_init) (void *), void (* instance_finalize) (void *));

ArikkeiClass *arikkei_type_get_class (unsigned int type);
unsigned int arikkei_type_is_a (unsigned int type, unsigned int test);
unsigned int arikkei_class_is_of_type (ArikkeiClass *klass, unsigned int type);

void arikkei_type_setup_instance (void *instance, unsigned int type);
void arikkei_type_release_instance (void *instance, unsigned int type);

const unsigned char *arikkei_type_get_name (unsigned int type);

/* Returns length of string not counting terminating zero, writes zero if there is room */
unsigned int arikkei_instance_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len);

ArikkeiProperty *arikkei_class_lookup_property (ArikkeiClass *klass, const unsigned char *key);
unsigned int arikkei_instance_set_property (ArikkeiClass *klass, void *instance, const unsigned char *key, const ArikkeiValue *val);
unsigned int arikkei_instance_get_property (ArikkeiClass *klass, void *instance, const unsigned char *key, ArikkeiValue *val);
unsigned int arikkei_instance_set_property_by_id (ArikkeiClass *klass, void *instance, unsigned int id, const ArikkeiValue *val);
unsigned int arikkei_instance_get_property_by_id (ArikkeiClass *klass, void *instance, unsigned int id, ArikkeiValue *val);

void *arikkei_instance_get_interface (ArikkeiClass *klass, void *instance, unsigned int type);

/* Setup helpers, for class implementations */

void arikkei_class_set_num_properties (ArikkeiClass *klass, unsigned int nproperties);
void arikkei_class_property_setup (ArikkeiClass *klass, unsigned int idx, const unsigned char *key, unsigned int type,
								   unsigned int isstatic, unsigned int canread, unsigned int canwrite, unsigned int isfinal, void *value);
void arikkei_class_method_setup (ArikkeiClass *klass, unsigned int idx, const unsigned char *key,
								 unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
								 unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *));
void arikkei_class_static_method_setup (ArikkeiClass *klass, unsigned int idx, const unsigned char *key,
										unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
										unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *));

#ifdef __cplusplus
};
#endif

#endif
