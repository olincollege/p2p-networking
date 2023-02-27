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

/* Sets a value in the hash table. The key and value is copied by value. */
void set_value(hash_table *in_table, void *key, size_t key_size, void *value,
               size_t value_size);

/* Removes a key from the hash_table or does nothing is such key does not exist
 */
void remove_kv_pair(hash_table *in_table, void *key, size_t key_size);

/* Returns a key/value pair in the hash_table or a NULL padded struct if it does
 * not exist */
kv_pair *get_kv_pair(hash_table *in_table, void *key, size_t key_size);

/* Returns a vector with all of the key/value pairs.
 * Note that the keys and values are pointers and may be invalidated with any
 * future hash_table operations.
 */
vector_kv_pair collect_table(hash_table *in_table);

/* Create a new hable */
hash_table make_table();

/* Free data used by the hash table */
void hash_dealloc(hash_table *in_table);

/* Internal */
void hash_realloc(hash_table *in_table);
