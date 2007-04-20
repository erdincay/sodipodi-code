#ifndef __ARIKKEI_DICT_H__
#define __ARIKKEI_DICT_H__

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

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ArikkeiDict ArikkeiDict;
typedef struct _ArikkeiDictEntry ArikkeiDictEntry;

struct _ArikkeiDict {
	unsigned int size;
	unsigned int hashsize;
	ArikkeiDictEntry *entries;
	int free;
	unsigned int (* hash) (const void *data);
	unsigned int (* equal) (const void *l, const void *r);
};

void arikkei_dict_setup_full (ArikkeiDict *dict, unsigned int hashsize,
			      unsigned int (* hash) (const void *),
			      unsigned int (* equal) (const void *, const void *));
void arikkei_dict_setup_string (ArikkeiDict *dict, unsigned int hashsize);
void arikkei_dict_setup_pointer (ArikkeiDict *dict, unsigned int hashsize);
void arikkei_dict_setup_int (ArikkeiDict *dict, unsigned int hashsize);
void arikkei_dict_release (ArikkeiDict *dict);

void arikkei_dict_insert (ArikkeiDict *dict, const void *key, const void *val);
void arikkei_dict_remove (ArikkeiDict *dict, const void *key);
const void *arikkei_dict_lookup (ArikkeiDict *dict, const void *key);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

// C++ wrapper

namespace Arikkei {

template <typename K, typename V>
class Dict {
private:
	ArikkeiDict _dict;
	// Forbidden
	Dict (const Dict& dict) { assert (false); }
	Dict& operator= (const Dict& dict) { assert (false); return *this; }
public:
	Dict (unsigned int hashsize, bool (* hash) (K), bool (* equal) (K, K)) {
		arikkei_dict_setup_full (&dict, hashsize, unsigned int (* hash) (const void *), unsigned int (* equal) (const void *, const void *));
	}
	V lookup (K key) { return (V) arikkei_dict_lookup (&_dict, key); }
	void insert (K key, V val) { arikkei_dict_insert (&_dict, key, val); }
};

template <typename V>
class Dict<const char *, V> {
private:
	ArikkeiDict _dict;
	// Forbidden
	Dict (const Dict& dict) { assert (false); }
	Dict& operator= (const Dict& dict) { assert (false); return *this; }
public:
	Dict (unsigned int hashsize) {
		arikkei_dict_setup_string (&_dict, hashsize);
	}
	V lookup (const char *key) { return (V) arikkei_dict_lookup (&_dict, key); }
	void insert (const char *key, V val) { arikkei_dict_insert (&_dict, key, val); }
};

template <typename V>
class Dict<int, V> {
private:
	ArikkeiDict _dict;
	// Forbidden
	Dict (const Dict& dict) { assert (false); }
	Dict& operator= (const Dict& dict) { assert (false); return *this; }
public:
	Dict (unsigned int hashsize) {
		arikkei_dict_setup_string (&_dict, hashsize);
	}
	const V lookup (const int key) { return (const V) arikkei_dict_lookup (&_dict, key); }
	void insert (const int key, const V val) { arikkei_dict_insert (&_dict, key, val); }
};

} // Namespace Arikkei

#endif

#endif
