#ifndef __NR_VALUES_H__
#define __NR_VALUES_H__

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

#define NR_EPSILON_D 1e-24
#define NR_EPSILON_F 1e-12F

#define NR_HUGE_D 1e24L
#define NR_HUGE_F 1e12F
#define NR_HUGE_L (0x7fffffff)
#define NR_HUGE_S (0x7fff)

#define NR_PI_F 3.14159265f
#define NR_2PI_F 6.28318531f
#define NR_PI2_F 1.57079633f

#define NR_PI_D 3.1415926535897932384626433832795
#define NR_2PI_D 6.283185307179586476925286766559
#define NR_PI2_D 1.5707963267948966192313216916398

#ifdef _WIN32
#define NR_EXPORT static
#else
#define NR_EXPORT
#endif

#ifndef __NR_VALUES_C__
extern NRRectL NR_RECT_L_EMPTY;
#endif

#if 1
#define NR_MATRIX_D_IDENTITY (*nr_matrix_d_get_identity ())
#define NR_MATRIX_F_IDENTITY (*nr_matrix_f_get_identity ())
#define NR_RECT_D_EMPTY (*nr_rect_d_get_empty ())
#define NR_RECT_F_EMPTY (*nr_rect_f_get_empty ())
#define NR_RECT_S_EMPTY (*nr_rect_s_get_empty ())
const NRMatrixD *nr_matrix_d_get_identity (void);
const NRMatrixF *nr_matrix_f_get_identity (void);
const NRRectD *nr_rect_d_get_empty (void);
const NRRectF *nr_rect_f_get_empty (void);
const NRRectL *nr_rect_l_get_empty (void);
const NRRectS *nr_rect_s_get_empty (void);
#else
#if defined (__NR_VALUES_C__) || defined (_WIN32)
NR_EXPORT NRMatrixD NR_MATRIX_D_IDENTITY = {{1.0, 0.0, 0.0, 1.0, 0.0, 0.0}};
NR_EXPORT NRMatrixF NR_MATRIX_F_IDENTITY = {{1.0, 0.0, 0.0, 1.0, 0.0, 0.0}};
NR_EXPORT NRRectD NR_RECT_D_EMPTY = {NR_HUGE_D, NR_HUGE_D, -NR_HUGE_D, -NR_HUGE_D};
NR_EXPORT NRRectF NR_RECT_F_EMPTY = {NR_HUGE_F, NR_HUGE_F, -NR_HUGE_F, -NR_HUGE_F};
NR_EXPORT NRRectL NR_RECT_L_EMPTY = {NR_HUGE_L, NR_HUGE_L, -NR_HUGE_L, -NR_HUGE_L};
NR_EXPORT NRRectS NR_RECT_S_EMPTY = {NR_HUGE_S, NR_HUGE_S, -NR_HUGE_S, -NR_HUGE_S};
#else
extern NRMatrixD NR_MATRIX_D_IDENTITY;
extern NRMatrixF NR_MATRIX_F_IDENTITY;
extern NRRectD NR_RECT_D_EMPTY;
extern NRRectF NR_RECT_F_EMPTY;
extern NRRectL NR_RECT_L_EMPTY;
extern NRRectS NR_RECT_S_EMPTY;
#endif
#endif

#ifdef __cplusplus
};
#endif

#endif
