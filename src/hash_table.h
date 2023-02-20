#include "./vector.h"

/* The key/value container for our hash map */
typedef struct kv_pair {
    char* key; 
    void* value;
    size_t value_size; 
} kv_pair; 

// create a bucket container for kv_pairs
vector__(kv_pair, kv_pair)

/* A hash table is an array of buckets */
typedef struct hash_table {
    vector_kv_pair* buckets;
    size_t bucket_size; 
    size_t num_elements;
} hash_table;

/* Sets a value in the hash table. The key and value is copied by value. */
void set_value(hash_table* in_table, char* key, void* value, size_t value_size); 

/* Returns a key/value pair in the hash_table or a NULL padded struct if it does not exist */
kv_pair* get_kv_pair(hash_table* in_table, char* key);


/* Create a new hable */
hash_table make_table(); 

/* Free data used by the hash table */
void hash_dealloc(hash_table* in_table);

/* Internal */
void hash_realloc(hash_table* in_table);

