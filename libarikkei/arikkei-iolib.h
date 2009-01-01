#ifndef __ARIKKEI_IOLIB_H__
#define __ARIKKEI_IOLIB_H__

/*
 * Arikkei
 *
 * Basic datatypes and code snippets
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 *
 */

#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif


const unsigned char *arikkei_mmap (const unsigned char *filename, size_t *size, const unsigned char *name);
void arikkei_munmap (const unsigned char *buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif
