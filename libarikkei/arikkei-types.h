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
typedef float f32;
typedef double f64;

/* Alignment */

#ifdef WIN32
#define ARIKKEI_A16 __declspec(align(16))
#else
#define ARIKKEI_A16 __attribute__ ((aligned (8)))
#endif

#endif
