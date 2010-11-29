#ifndef __LIBSODIPODI_PREFERENCES_H__
#define __LIBSODIPODI_PREFERENCES_H__

//
// Libsodipodi
//
// Copyright (C) Lauris Kaplinski 2000-2009
//

#include <thera/thera-node.h>

TheraNode *sp_config_node_get (const unsigned char *path, unsigned int create);

const unsigned char *sp_config_value_get (const unsigned char *path, const unsigned char *key, const unsigned char *defval);

unsigned int sp_config_value_get_boolean (const unsigned char *path, const unsigned char *key, unsigned int defval);
int sp_config_value_get_int (const unsigned char *path, const unsigned char *key, int defval);
double sp_config_value_get_double (const unsigned char *path, const unsigned char *key, double defval);
unsigned int sp_config_value_set (const unsigned char *path, const unsigned char *key, const unsigned char *val, unsigned int create);
unsigned int sp_config_value_set_boolean (const unsigned char *path, const unsigned char *key, unsigned int val, unsigned int create);
unsigned int sp_config_value_set_int (const unsigned char *path, const unsigned char *key, int val, unsigned int create);
unsigned int sp_config_value_set_double (const unsigned char *path, const unsigned char *key, double val, unsigned int create);

#endif
