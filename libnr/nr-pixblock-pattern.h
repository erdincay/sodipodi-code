#ifndef __NR_PIXBLOCK_PATTERN_H__
#define __NR_PIXBLOCK_PATTERN_H__

/*
 * Pixel buffer rendering library
 *
 * Authors:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#include <libnr/nr-pixblock.h>

#ifdef __cplusplus
extern "C" {
#endif

void nr_pixblock_fill (NRPixBlock *pb, unsigned int rgba32);
void nr_pixblock_render_gray_noise (NRPixBlock *pb, NRPixBlock *mask);

#ifdef __cplusplus
};
#endif

#endif
