#ifndef __ARIKKEI_CACHE_TEMPLATE_H__
#define __ARIKKEI_CACHE_TEMPLATE_H__

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

#include <libarikkei/arikkei-cache.h>

namespace Arikkei {

template <typename K, typename V>
class Cache {
private:
	ArikkeiCache _cache;
	// Forbidden
	Cache (const Cache& cache) { assert (false); }
	Cache& operator= (const Cache& cache) { assert (false); return *this; }
public:
	Cache (unsigned int size, bool (* key_hash) (K), bool (* key_equal) (K, K), K (* key_dup) (const K), void (* key_free) (K), void (* object_free) (V)) {
		arikkei_cache_setup_full (&_cache, size, unsigned int (* hash) (const void *), unsigned int (* equal) (const void *, const void *),
			void *(* key_dup) (const void *key), void (* key_free) (void *key), void (* object_free) (const void *object));
	}
	void insert (K key, V object, unsigned int size) { arikkei_cache_insert (&_cache, key, object, size};
	void remove (K key) { arikkei_cache_remove (&_cache, key};

	V lookup (K key) { return (V) arikkei_cache_lookup (&_cache, key); }
	V lookup_notouch (K key) { return (V) arikkei_cache_lookup_notouch (&_cache, key); }
};

} // Namespace Arikkei

#endif





