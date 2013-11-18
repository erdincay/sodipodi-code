#ifndef __ARIKKEI_UTILS_H__
#define __ARIKKEI_UTILS_H__

/*
 * Miscellaneous utilities
 *
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *
 * This code is in public domain
 */

#ifdef __cplusplus
extern "C" {
#endif

#define arikkei_return_if_fail(expr) if (!(expr) && arikkei_emit_fail_warning ((const unsigned char *) __FILE__, __LINE__, (const unsigned char *) "?", (const unsigned char *) #expr)) return
#define arikkei_return_val_if_fail(expr,val) if (!(expr) && arikkei_emit_fail_warning ((const unsigned char *) __FILE__, __LINE__, (const unsigned char *) "?", (const unsigned char *) #expr)) return (val)

unsigned int arikkei_emit_fail_warning (const unsigned char *file, unsigned int line, const unsigned char *method, const unsigned char *expr);

#ifdef __cplusplus
};
#endif

#endif

