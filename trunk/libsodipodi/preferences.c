#define __LIBSODIPODI_PREFERENCES_CPP__

//
// Libsodipodi
//
// Copyright (C) Lauris Kaplinski 2000-2009
//

#include <libarikkei/arikkei-token.h>

#include "preferences.h"

#define SP_MAX_CONFIG_DEPTH 64
#define SP_MAX_CONFIG_ID_LENGTH 256

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

TheraNode *
sp_config_node_get (const unsigned char *path, unsigned int create)
{
#if 0
	ArikkeiToken tokens[SP_MAX_CONFIG_DEPTH];
	ArikkeiToken ptoken;
	unsigned int ntokens;
	TheraNode *repr;
	unsigned int pos;

	if (!path) return NULL;
	if (!*path) return NULL;

	/* Tokenize path */
	arikkei_token_set_from_string (&ptoken, path);
	ntokens = arikkei_token_tokenize_ws (&ptoken, tokens, SP_MAX_CONFIG_DEPTH, ".", 0);
	if (!ntokens) return NULL;

	if (!arikkei_token_strcmp (&tokens[0], "extensions")) {
		repr = sp_repr_doc_get_root (sodipodi->extensions);
	} else {
		repr = sp_repr_doc_get_root (sodipodi->preferences);
	}
	assert (!(strcmp (sp_repr_get_name (repr), "sodipodi")));

	/* Parse all tokens */
	pos = 0;
	while (pos < ntokens) {
		unsigned char c[SP_MAX_CONFIG_ID_LENGTH];
		SPRepr *child;

		arikkei_token_strncpy (&tokens[pos], c, SP_MAX_CONFIG_ID_LENGTH);
		if (!c[0]) return NULL;
		child = sp_repr_lookup_child (repr, "id", c);
		if (!child && create) {
			child = sp_repr_new ("node");
			sp_repr_set_attr (child, "id", c);
			sp_repr_append_child (repr, child);
		}
		if (!child) return NULL;
		repr = child;
		pos += 1;
	}
	return repr;
#endif
	return NULL;
}

const unsigned char *
sp_config_value_get (const unsigned char *path, const unsigned char *key, const unsigned char *defval)
{
	TheraNode *repr;
	const unsigned char *val;
	repr = sp_config_node_get (path, FALSE);
	if (!repr) return defval;
	val = thera_node_get_attribute (repr, key);
	return (val) ? val : defval;
}

unsigned int
sp_config_value_get_boolean (const unsigned char *path, const unsigned char *key, unsigned int defval)
{
#if 0
	TheraNode *repr;
	repr = sp_config_node_get (path, FALSE);
	if (!repr) return defval;
	sp_repr_get_boolean (repr, key, &defval);
	return defval;
#endif
	return defval;
}

int
sp_config_value_get_int (const unsigned char *path, const unsigned char *key, int defval)
{
#if 0
	TheraNode *repr;
	repr = sp_config_node_get (path, FALSE);
	if (!repr) return defval;
	sp_repr_get_int (repr, key, &defval);
	return defval;
#endif
	return defval;
}

double
sp_config_value_get_double (const unsigned char *path, const unsigned char *key, double defval)
{
#if 0
	TheraNode *repr;
	repr = sp_config_node_get (path, FALSE);
	if (!repr) return defval;
	sp_repr_get_double (repr, key, &defval);
	return defval;
#endif
	return defval;
}

unsigned int
sp_config_value_set (const unsigned char *path, const unsigned char *key, const unsigned char *val, unsigned int create)
{
#if 0
	TheraNode *repr;
	repr = sp_config_node_get (path, create);
	if (!repr) return FALSE;
	return sp_repr_set_attr (repr, key, val);
#endif
	return FALSE;
}

unsigned int
sp_config_value_set_boolean (const unsigned char *path, const unsigned char *key, unsigned int val, unsigned int create)
{
#if 0
	TheraNode *repr;
	repr = sp_config_node_get (path, create);
	if (!repr) return FALSE;
	return sp_repr_set_boolean (repr, key, val);
#endif
	return FALSE;
}

unsigned int
sp_config_value_set_int (const unsigned char *path, const unsigned char *key, int val, unsigned int create)
{
#if 0
	TheraNode *repr;
	repr = sp_config_node_get (path, create);
	if (!repr) return FALSE;
	return sp_repr_set_int (repr, key, val);
#endif
	return FALSE;
}

unsigned int
sp_config_value_set_double (const unsigned char *path, const unsigned char *key, double val, unsigned int create)
{
#if 0
	TheraNode *repr;
	repr = sp_config_node_get (path, create);
	if (!repr) return FALSE;
	return sp_repr_set_double (repr, key, val);
#endif
	return FALSE;
}
