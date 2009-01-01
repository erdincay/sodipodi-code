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





