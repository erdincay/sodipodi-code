#ifndef __ARIKKEI_INTERFACE_H__
#define __ARIKKEI_INTERFACE_H__

/*
 * Interface class
 *
 * Copyright Lauris Kaplinski <lauris@kaplinski.com> 2014
 * 
 */

#include <libarikkei/arikkei-types.h>

#include <az/interface.h>

/*
 * Interface has three parts
 *   1 Class
 *   2 Class implementation in containing type class
 *   3 Instance in containing type instance
 */

#ifdef __cplusplus
extern "C" {
#endif

#define arikkei_register_interface_type az_register_interface_type

#ifdef __cplusplus
};
#endif

#endif

