#define __ARIKKEI_DICT_C__

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

#include <malloc.h>
#include <string.h>

#include "arikkei-dict.h"

struct _ArikkeiDictEntry {
	unsigned int next;
	const void *key;
	const void *val;
};

void
arikkei_dict_setup_full (ArikkeiDict *dict, unsigned int hashsize,
			 unsigned int (* hash) (const void *),
			 unsigned int (* equal) (const void *, const void *))
{
	unsigned int i;
	if (hashsize < 1) hashsize = 1;
	dict->root_size = hashsize;
	dict->size = 3 * hashsize;
	dict->entries = (ArikkeiDictEntry *) malloc (dict->size * sizeof (ArikkeiDictEntry));
	memset (dict->entries, 0, dict->size * sizeof (ArikkeiDictEntry));
	for (i = dict->root_size; i < dict->size - 1; i++) dict->entries[i].next = i + 1;
	dict->entries[dict->size - 1].next = 0;
	dict->free = dict->root_size;
	dict->hash = hash;
	dict->equal = equal;
}

static void
dict_reallocate (ArikkeiDict *dict, unsigned int hash_size)
{
	ArikkeiDictEntry *entries;
	unsigned int size, next_free, i;
	size = 3 * hash_size;
	entries = (ArikkeiDictEntry *) malloc (size * sizeof (ArikkeiDictEntry));
	memset (entries, 0, size * sizeof (ArikkeiDictEntry));
	for (i = hash_size; i < size - 1; i++) entries[i].next = i + 1;
	entries[size - 1].next = 0;
	next_free = hash_size;
	for (i = 0; i < dict->root_size; i++) {
		unsigned int pos;
		if (!dict->entries[i].key) continue;
		pos = i;
		do {
			unsigned int hval = dict->hash (dict->entries[pos].key) % hash_size;
			if (!entries[hval].key) {
				entries[hval].key = dict->entries[pos].key;
				entries[hval].val = dict->entries[pos].val;
			} else {
				unsigned int next = next_free;
				next_free = entries[next_free].next;
				entries[next].key = dict->entries[pos].key;
				entries[next].val = dict->entries[pos].val;
				entries[next].next = entries[hval].next;
				entries[hval].next = next;
			}
		} while (pos);
	}
	free (dict->entries);
	dict->entries = entries;
	dict->root_size = hash_size;
	dict->size = size;
	dict->free = next_free;
}

void
arikkei_dict_setup_string (ArikkeiDict *dict, unsigned int hashsize)
{
	arikkei_dict_setup_full (dict, hashsize, arikkei_string_hash, arikkei_string_equal);
}

void
arikkei_dict_setup_pointer (ArikkeiDict *dict, unsigned int hashsize)
{
	arikkei_dict_setup_full (dict, hashsize, arikkei_pointer_hash, arikkei_pointer_equal);
}

void
arikkei_dict_setup_int (ArikkeiDict *dict, unsigned int hashsize)
{
	arikkei_dict_setup_full (dict, hashsize, arikkei_int_hash, arikkei_int_equal);
}

void
arikkei_dict_release (ArikkeiDict *dict)
{
	free (dict->entries);
}

void
arikkei_dict_insert (ArikkeiDict *dict, const void *key, const void *val)
{
	unsigned int root, pos;
	if (!key) return;
	root = dict->hash (key) % dict->root_size;
	if (!dict->entries[root].key) {
		/* Add root element */
		dict->entries[root].key = key;
		dict->entries[root].val = val;
		return;
	}
	pos = root;
	do {
		if (dict->equal (key, dict->entries[pos].key)) {
			/* Replace element */
			dict->entries[pos].key = key;
			dict->entries[pos].val = val;
			return;
		}
		pos = dict->entries[pos].next;
	} while (pos);
	/* Prepend new element */
	if (dict->free) {
		pos = dict->free;
		dict->free = dict->entries[pos].next;
		dict->entries[pos].key = key;
		dict->entries[pos].val = val;
		dict->entries[pos].next = dict->entries[root].next;
		dict->entries[root].next = pos;
		return;
	}
	dict_reallocate (dict, dict->root_size << 1);
	root = dict->hash (key) % dict->root_size;
	if (!dict->entries[root].key) {
		/* Add root element */
		dict->entries[root].key = key;
		dict->entries[root].val = val;
		return;
	}
	pos = dict->free;
	dict->free = dict->entries[pos].next;
	dict->entries[pos].key = key;
	dict->entries[pos].val = val;
	dict->entries[pos].next = dict->entries[root].next;
	dict->entries[root].next = pos;
}

void
arikkei_dict_remove (ArikkeiDict *dict, const void *key)
{
	unsigned int root;
	if (!key) return;
	root = dict->hash (key) % dict->root_size;
	if (!dict->entries[root].key) return;
	if (dict->equal (dict->entries[root].key, key)) {
		/* Have to remove root key */
		if (dict->entries[root].next) {
			int pos;
			pos = dict->entries[root].next;
			dict->entries[root] = dict->entries[pos];
			dict->entries[pos].next = dict->free;
			dict->free = pos;
		} else {
			dict->entries[root].key = NULL;
		}
	} else {
		int pos, prev;
		prev = root;
		for (pos = dict->entries[root].next; pos; pos = dict->entries[pos].next) {
			if (dict->equal (dict->entries[pos].key, key)) {
				dict->entries[prev].next = dict->entries[pos].next;
				dict->entries[pos].next = dict->free;
				dict->free = pos;
				return;
			}
			prev = pos;
		}
	}
}

void
arikkei_dict_clear (ArikkeiDict *dict)
{
	unsigned int i;
	for (i = 0; i < dict->root_size; i++) dict->entries[i].key = NULL;
	for (i = dict->root_size; i < dict->size - 1; i++) dict->entries[i].next = i + 1;
	dict->entries[dict->size - 1].next = 0;
	dict->free = dict->root_size;
}

unsigned int
arikkei_dict_exists (ArikkeiDict *dict, const void *key)
{
	unsigned int hval, pos;
	if (!key) return 0;
	hval = dict->hash (key) % dict->root_size;
	if (!dict->entries[hval].key) return 0;
	pos = hval;
	do {
		if (dict->equal (dict->entries[pos].key, key)) return 1;
		pos = dict->entries[pos].next;
	} while (pos);
	return 0;
}

const void *
arikkei_dict_lookup (ArikkeiDict *dict, const void *key)
{
	unsigned int hval, pos;
	if (!key) return NULL;
	hval = dict->hash (key) % dict->root_size;
	if (!dict->entries[hval].key) return NULL;
	pos = hval;
	do {
		if (dict->equal (dict->entries[pos].key, key)) return dict->entries[pos].val;
		pos = dict->entries[pos].next;
	} while (pos);
	return NULL;
}

const void *
arikkei_dict_lookup_foreign (ArikkeiDict *dict, const void *key, unsigned int (*hash) (const void *key), unsigned int (*equal) (const void *lhs, const void *rhs))
{
	unsigned int hval, pos;
	if (!key) return NULL;
	hval = hash (key) % dict->root_size;
	if (!dict->entries[hval].key) return NULL;
	pos = hval;
	do {
		if (equal (key, dict->entries[pos].key)) return dict->entries[pos].val;
		pos = dict->entries[pos].next;
	} while (pos);
	return NULL;
}

unsigned int
arikkei_dict_forall (ArikkeiDict *dict, unsigned int (* forall) (const void *, const void *, void *), void *data)
{
	unsigned int i, pos;
	for (i = 0; i < dict->root_size; i++) {
		if (!dict->entries[i].key) continue;
		pos = i;
		do {
			if (!forall (dict->entries[pos].key, dict->entries[pos].val, data)) return 0;
			pos = dict->entries[pos].next;
		} while (pos);
	}
	return 1;
}

unsigned int
arikkei_dict_remove_all (ArikkeiDict *dict, unsigned int (*remove) (const void *, const void *, void *), void *data)
{
	unsigned int hash, n_removed;
	n_removed = 0;
	for (hash = 0; hash < dict->root_size; hash++) {
		if (!dict->entries[hash].key) continue;
		if (remove (dict->entries[hash].key, dict->entries[hash].val, data)) {
			/* Remove root entry */
			if (dict->entries[hash].next > 0) {
				int pos;
				pos = dict->entries[hash].next;
				dict->entries[hash] = dict->entries[pos];
				dict->entries[pos].next = dict->free;
				dict->free = pos;
			} else {
				dict->entries[hash].key = NULL;
			}
			n_removed += 1;
		} else {
			int pos, prev;
			prev = hash;
			pos = dict->entries[hash].next;
			while (pos) {
				if (remove (dict->entries[pos].key, dict->entries[pos].val, data)) {
					dict->entries[prev].next = dict->entries[pos].next;
					dict->entries[pos].next = dict->free;
					dict->free = pos;
					n_removed += 1;
				} else {
					prev = pos;
					pos = dict->entries[pos].next;
				}
			}

		}
	}
	return n_removed;
}

unsigned int
arikkei_string_hash (const void *data)
{
	const unsigned char *p;
	unsigned int hval;
	p = data;
	hval = *p;
	if (hval) {
		for (p = p + 1; *p; p++) hval = (hval << 5) - hval + *p;
	}
	return hval;
}

unsigned int
arikkei_string_equal (const void *l, const void *r)
{
	return !strcmp (l, r);
}

unsigned int
arikkei_pointer_hash (const void *data)
{
	return arikkei_memory_hash (&data, sizeof (data));
}

unsigned int
arikkei_pointer_equal (const void *l, const void *r)
{
	return l == r;
}

unsigned int
arikkei_int_hash (const void *data)
{
	unsigned int hval, p;
	hval = 0;
	p = (unsigned int) ((const char *) data - (const char *) 0);
	while (p) {
		hval ^= p;
		p /= 17;
	}
	return hval;
}

unsigned int
arikkei_int_equal (const void *l, const void *r)
{
	return (unsigned int) ((const char *) l - (const char *) 0) == (unsigned int) ((const char *) r - (const char *) 0);
}

unsigned int
arikkei_memory_hash (const void *data, unsigned int size)
{
	const unsigned char *p;
	unsigned int hval, i;
	p = data;
	hval = *p;
	for (i = 1; i < size; i++) {
		hval = (hval << 5) - hval + p[i];
	}
	return hval;
}

unsigned int
arikkei_memory_equal (const void *l, const void *r, unsigned int size)
{
	unsigned int i;
	for (i = 0; i < size; i++) {
		if (*((const char *) l + i) != *((const char *) r + i)) return 0;
	}
	return 1;
}


