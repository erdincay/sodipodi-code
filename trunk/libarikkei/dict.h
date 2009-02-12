#ifndef __ARIKKEI_DICT_TEMPLATE_H__
#define __ARIKKEI_DICT_TEMPLATE_H__

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

#include <libarikkei/arikkei-dict.h>

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
		arikkei_dict_setup_full (&_dict, hashsize, unsigned int (* hash) (const void *), unsigned int (* equal) (const void *, const void *));
	}
	V lookup (K key) { return (V) arikkei_dict_lookup (&_dict, (const void *) key); }
	void insert (K key, V val) { arikkei_dict_insert (&_dict, (const void *) key, (const void *) val); }
	void remove (K key) { arikkei_dict_remove (&_dict, (const void *) key); }
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
	V lookup (const char *key) { return (V) arikkei_dict_lookup (&_dict, (const void *) key); }
	void insert (const char *key, V val) { arikkei_dict_insert (&_dict, (const void *) key, (const void *) val); }
	void remove (const char *key) { arikkei_dict_remove (&_dict, (const void *) key); }
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
		arikkei_dict_setup_int (&_dict, hashsize);
	}
	const V lookup (int key) { return (const V) arikkei_dict_lookup (&_dict, (const void *) key); }
	void insert (int key, const V val) { arikkei_dict_insert (&_dict, (const void *) key, (const void *) val); }
	void remove (int key) { arikkei_dict_remove (&_dict, (const void *) key); }
};

} // Namespace Arikkei

#endif




