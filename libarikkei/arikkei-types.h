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

typedef struct _ArikkeiClass ArikkeiClass;
typedef struct _ArikkeiInterfaceClass ArikkeiInterfaceClass;
typedef struct _ArikkeiInterfaceImplementation ArikkeiInterfaceImplementation;

typedef struct _ArikkeiValue ArikkeiValue;
typedef struct _ArikkeiReference ArikkeiReference;
typedef struct _ArikkeiString ArikkeiString;
typedef struct _ArikkeiProperty ArikkeiProperty;
typedef struct _ArikkeiObject ArikkeiObject;

/* Alignment */

#ifdef _WIN32
#define ARIKKEI_A16 __declspec(align(16))
#else
#define ARIKKEI_A16 __attribute__ ((aligned (8)))
#endif

/* Typecodes */

enum {
	/* Invalid or missing type, typecode 0 */
	ARIKKEI_TYPE_NONE,
	/* Universal base class */
	ARIKKEI_TYPE_ANY,
	/* True primitives */
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
	/* Special types */
	ARIKKEI_TYPE_CLASS,
	ARIKKEI_TYPE_INTERFACE,
	ARIKKEI_TYPE_ARRAY,
	ARIKKEI_TYPE_REFERENCE,

	ARIKKEI_TYPE_STRING,
	ARIKKEI_TYPE_VALUE,
	ARIKKEI_TYPE_NUM_PRIMITIVES
};

#define ARIKKEI_OFFSET(b,m) ARIKKEI_POINTER_TO_INT(&((b *) 0)->m)
#define ARIKKEI_INT_TO_POINTER(v) (void *) ((char *) 0 + (v))
#define ARIKKEI_POINTER_TO_INT(p) ((int) ((char *) p - (char *) 0))

#define ARIKKEI_TYPE_IS_ARITHMETIC(t) (((t) >= ARIKKEI_TYPE_INT8) && ((t) <= ARIKKEI_TYPE_DOUBLE))
#define ARIKKEI_TYPE_IS_INTEGRAL(t) (((t) >= ARIKKEI_TYPE_INT8) && ((t) <= ARIKKEI_TYPE_UINT64))
#define ARIKKEI_TYPE_IS_SIGNED(t) (((t) == ARIKKEI_TYPE_INT8) || ((t) == ARIKKEI_TYPE_INT16) || ((t) == ARIKKEI_TYPE_INT32) || ((t) == ARIKKEI_TYPE_INT64) || ((t) == ARIKKEI_TYPE_FLOAT) || ((t) == ARIKKEI_TYPE_DOUBLE))
#define ARIKKEI_TYPE_IS_UNSIGNED(t) (((t) == ARIKKEI_TYPE_UINT8) || ((t) == ARIKKEI_TYPE_UINT16) || ((t) == ARIKKEI_TYPE_UINT32) || ((t) == ARIKKEI_TYPE_UINT64))
#define ARIKKEI_TYPE_IS_64(t) (((t) == ARIKKEI_TYPE_INT64) || ((t) == ARIKKEI_TYPE_UINT64))

ARIKKEI_A16 struct _ArikkeiClass {
	unsigned int type;
	ArikkeiClass *parent;

	/* All implemented interfaces */
	unsigned int firstinterface;
	unsigned int ninterfaces;
	unsigned int *implementations;

	unsigned int firstproperty;
	unsigned int nproperties;
	ArikkeiProperty *properties;

	const unsigned char *name;
	/* Size of class structure */
	unsigned int class_size;
	/* Size of instance */
	unsigned int instance_size;
	/* Size of instance in arrays (rounded to 16 bytes for aligned types) */
	unsigned int element_size;
	/* Subclasses should not remove flag set by parent */
	unsigned int zero_memory : 1;

	/* Memory management */
	void *(* allocate) (ArikkeiClass *klass);
	void *(* allocate_array) (ArikkeiClass *klass, unsigned int nelements);
	void (* free) (ArikkeiClass *klass, void *location);
	void (* free_array) (ArikkeiClass *klass, void *location, unsigned int nelements);
	/* Constructors and destructors */
	void (* instance_init) (void *instance);
	void (* instance_finalize) (void *instance);
	/* Duplicate creates a copy in uninitialized memory */
	void (* duplicate) (ArikkeiClass *klass, void *destination, void *instance);
	/* Assign overwrites existing initialized instance */
	void (* assign) (ArikkeiClass *klass, void *destination, void *instance);

	unsigned int (* to_string) (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len);
	unsigned int (*get_property) (ArikkeiClass *klass, void *instance, unsigned int idx, ArikkeiValue *val);
	unsigned int (*set_property) (ArikkeiClass *klass, void *instance, unsigned int idx, const ArikkeiValue *val);
	/* void *(* get_interface) (ArikkeiClass *klass, void *instance, unsigned int type); */
};

void arikkei_types_init (void);

/* For special subtype implementations */
/* Register class in type system */
/* Class has to be zeroed and relevant fields of subclasses set up */
/* Type is guaranteed to be assigned before class constructors are invoked */
void arikkei_register_class (ArikkeiClass *klass, unsigned int *type, unsigned int parent, const unsigned char *name, unsigned int class_size, unsigned int instance_size,
							void (* class_init) (ArikkeiClass *), void (* instance_init) (void *), void (* instance_finalize) (void *));
/* Returns allocated class */
ArikkeiClass *arikkei_register_type (unsigned int *type, unsigned int parent, const unsigned char *name, unsigned int class_size, unsigned int instance_size,
							void (* class_init) (ArikkeiClass *), void (* instance_init) (void *), void (* instance_finalize) (void *));

ArikkeiClass *arikkei_type_get_class (unsigned int type);

unsigned int arikkei_type_is_a (unsigned int type, unsigned int test);
unsigned int arikkei_type_implements_a (unsigned int type, unsigned int test);
/* Returns true if either test is object and type is a subtype or test is interface and object implements it */
unsigned int arikkei_type_is_assignable_to (unsigned int type, unsigned int test);
/* Returns most specific builtin parent type for this object */
unsigned int arikkei_type_get_parent_primitive (unsigned int type);

unsigned int arikkei_class_is_of_type (ArikkeiClass *klass, unsigned int type);

void *arikkei_instance_new (unsigned int type);
void *arikkei_instance_new_array (unsigned int type, unsigned int nelements);
void arikkei_instance_delete (unsigned int type, void *instance);
void arikkei_instance_delete_array (unsigned int type, void *elements, unsigned int nelements);
void arikkei_instance_setup (void *instance, unsigned int type);
void arikkei_instance_release (void *instance, unsigned int type);

const unsigned char *arikkei_type_get_name (unsigned int type);

/* Returns length of string not counting terminating zero, writes zero if there is room */
unsigned int arikkei_instance_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len);

ArikkeiInterfaceImplementation *arikkei_class_get_interface_implementation (ArikkeiClass *klass, unsigned int type);

ArikkeiProperty *arikkei_class_lookup_property (ArikkeiClass *klass, const unsigned char *key);
unsigned int arikkei_instance_set_property (ArikkeiClass *klass, void *instance, const unsigned char *key, const ArikkeiValue *val);
unsigned int arikkei_instance_get_property (ArikkeiClass *klass, void *instance, const unsigned char *key, ArikkeiValue *val);
unsigned int arikkei_instance_set_property_by_id (ArikkeiClass *klass, void *instance, unsigned int id, const ArikkeiValue *val);
unsigned int arikkei_instance_get_property_by_id (ArikkeiClass *klass, void *instance, unsigned int id, ArikkeiValue *val);

/* Setup helpers, for class implementations */

void arikkei_class_set_num_interfaces (ArikkeiClass *klass, unsigned int ninterfaces);
void arikkei_interface_implementation_setup (ArikkeiClass *klass, unsigned int idx, unsigned int type, unsigned int class_offset, unsigned int instance_offset);

void arikkei_class_set_num_properties (ArikkeiClass *klass, unsigned int nproperties);
void arikkei_class_property_setup (ArikkeiClass *klass, unsigned int idx, const unsigned char *key, unsigned int type,
							  unsigned int is_static, unsigned int can_read, unsigned int can_write, unsigned int is_final, unsigned int is_value,
							  unsigned int value_type, void *value);
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
