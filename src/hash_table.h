#pragma once

#include "./vector.h"

/* The key/value container for our hash map */
typedef struct kv_pair {
  void *key;
  void *value;
  size_t key_size;
  size_t value_size;
} kv_pair;

// create a bucket container for kv_pairs
vector__(kv_pair, kv_pair)

    /* A hash table is an array of buckets */
    typedef struct hash_table {
  vector_kv_pair *buckets;
  size_t bucket_size;
  size_t num_elements;
} hash_table;

/**
 * Set a value in the hash table. The key and value is copied by value.
 *
 * @param in_table Location of the hash table in memory
 * @param key Location of binary data to hash as a key
 * @param key_size Number of bytes the key occupies in memory
 * @param value Location of data to copy into the hash table
 * @param value_size Number of bytes the value occupies in memory
 */
void set_value(hash_table *in_table, void *key, size_t key_size, void *value,
               size_t value_size);

/**
 * Remove a key/value pair from the hash table.
 *
 * If key is not in the hash table, does nothing.
 *
 * @param in_table Location of the hash table in memory
 * @param key Location of binary data to hash as a key
 * @param key_size Number of bytes the key occupies in memory
 */
void remove_kv_pair(hash_table *in_table, void *key, size_t key_size);

/**
 * Return a key/value pair in the hash_table.
 *
 * If key/value pair does not exist, returns a NULL padded struct.
 *
 * @param in_table Location of the hash table in memory
 * @param key Location of binary data to hash as a key
 * @param key_size Number of bytes the key occupies in memory
 */
kv_pair *get_kv_pair(hash_table *in_table, void *key, size_t key_size);

/**
 * Return a vector with all of the key/value pairs.
 *
 * Note that the keys and values are pointers and may be invalidated with any
 * future hash_table operations.
 *
 * @param in_table Location of the hash table in memory
 * @return A vector with all the key/value pairs currently in the hash table.
 */
vector_kv_pair collect_table(hash_table *in_table);

/**
 * Create a new hash table.
 *
 * @return An initialized, empty hash table object.
 */
hash_table make_table(void);

/* For internal use with make_table */
static hash_table make_table__(size_t bucket_size);

/**
 * Free data used by the hash table.
 *
 * @param in_table Location of the hash table in memory
 */
void hash_dealloc(hash_table *in_table);

/* Internal */
void hash_realloc(hash_table *in_table);
