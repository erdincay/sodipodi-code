#ifndef __NR_PIXBLOCK_COMPOSE_H__
#define __NR_PIXBLOCK_COMPOSE_H__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <libnr/nr-types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* We use inverse matrix here instead of inverting it each time */
void nr_pixblock_compose (NRPixBlock *d, const NRPixBlock *s, unsigned int alpha, const NRMatrixF *d2s);

#ifdef __cplusplus
};
#endif

#endif
