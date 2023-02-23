#include "./hash.h"
#include "./hash_table.h"
#include <string.h>

/* Internal, hash table with custom initial bucket size */
hash_table make_table__(size_t bucket_size) {
    hash_table new_table = {malloc(sizeof(vector_kv_pair) * bucket_size), bucket_size, 0};
    for(size_t vec = 0; vec < bucket_size; vec++) {
        new_table.buckets[vec] = new_vec_kv_pair();
    }
    return new_table;
}
/* Creates a new empty hash table */
hash_table make_table() {
    return make_table__(2);
}

/* Returns a key/value pair in the hash_table or a NULL padded struct if it does not exist */
kv_pair* get_kv_pair(hash_table* in_table, void* key, size_t key_size) {
    // find the bucket for the key
    size_t bucket = djb2_bytes(key, key_size) % in_table->bucket_size;    
    // walk the bucket
    for(size_t index = 0; index < (in_table->buckets[bucket].size); index++) {
        char* ret_key = in_table->buckets[bucket].arr[index].key; 
        size_t ret_key_size = in_table->buckets[bucket].arr[index].key_size;
        if(ret_key_size == key_size && memcmp(key, ret_key, key_size) == 0) {
            return &in_table->buckets[bucket].arr[index];
        }
    }
    return NULL;
}

/* Removes a key from the hash_table or does nothing is such key does not exist */
void remove_kv_pair(hash_table* in_table, void* key, size_t key_size) {
    // find the bucket for the key
    size_t bucket = djb2_bytes(key, key_size) % in_table->bucket_size;    
    vector_kv_pair without_key = new_vec_kv_pair();
    // walk the bucket 
    for(size_t index = 0; index < (in_table->buckets[bucket].size); index++) {
        kv_pair pair = in_table->buckets[bucket].arr[index]; 
        char* ret_key = pair.key;
        size_t ret_key_size = pair.key_size; 
        if((ret_key_size == key_size && memcmp(key, ret_key, key_size) == 0)) {
            // remove element 
            free(pair.key);
            free(pair.value);
            in_table->num_elements --; 
        }
        else {
            // keep key/value pair 
            push_vec_kv_pair(&without_key, pair);
        }
    }
    // swap the bucket array
    free_vec_kv_pair(&in_table->buckets[bucket]);
    in_table->buckets[bucket] = without_key;
}

/* Returns a vector with all of the key/value pairs.
 * Note that the keys and values are pointers and may be invalidated with any future hash_table operations. 
 */
vector_kv_pair collect_table(hash_table* in_table) {
    vector_kv_pair collected = new_vec_kv_pair();
    // walk the buckets
    for(size_t bucket = 0; bucket < in_table->bucket_size; bucket++) {
        // walk the elements
        for(size_t elem = 0; elem < in_table->buckets[bucket].size; elem++) {
            push_vec_kv_pair(&collected, in_table->buckets[bucket].arr[elem]);
        }
    }
    return collected;
}

/* Frees the data used by the hash table */
void hash_dealloc(hash_table* in_table) {
    for(size_t bucket = 0; bucket < in_table->bucket_size; bucket++) {
        for(size_t elem = 0; elem < in_table->buckets[bucket].size; elem ++) {
            free(in_table->buckets[bucket].arr[elem].key);
            free(in_table->buckets[bucket].arr[elem].value);
        }
        free_vec_kv_pair(&in_table->buckets[bucket]);
    }
    free(in_table->buckets);
    in_table->buckets = NULL;
}

/* Reallocs the hash table to handle more elements */
void hash_realloc(hash_table* in_table) { // NOLINT recursive resolve is fine here
                                          // allocate a new table with double the bucket size
    hash_table new_table = make_table__(in_table->bucket_size * 2);
    // copy elements in
    for(size_t bucket = 0; bucket < in_table->bucket_size; bucket ++) {
        for(size_t element = 0; element < in_table->buckets[element].size; element++) {
            kv_pair cur_pair = in_table->buckets[bucket].arr[element]; 
            set_value(&new_table, cur_pair.key, cur_pair.key_size, cur_pair.value, cur_pair.value_size); // NOLINT
        }
    }
    // swap new table with input table
    hash_dealloc(in_table);
    in_table->buckets = new_table.buckets; 
    in_table->bucket_size *= 2;
}

/* Updates the value of a key in a hash table */
void set_value(hash_table* in_table, void* key, size_t key_size, void* value, size_t value_size) { // NOLINT recursive resolve is fine here
    kv_pair* get_pair = get_kv_pair(in_table, key, key_size);     
    if(get_pair) {
        free(get_pair->value); 
        get_pair->value = malloc(value_size); 
        memcpy(get_pair->value, value, value_size); // NOLINT
    }
    else {
        if(in_table->num_elements == in_table->bucket_size) {
            hash_realloc(in_table); // NOLINT
        }
        in_table->num_elements ++;
        size_t bucket = djb2_bytes(key, key_size) % in_table->bucket_size;  // NOLINT
        kv_pair new_pair = {malloc(key_size), malloc(value_size), key_size, value_size};
        memcpy(new_pair.key, key, key_size); // NOLINT
        memcpy(new_pair.value, value, value_size); // NOLINT
        push_vec_kv_pair(&in_table->buckets[bucket], new_pair);
    }
}
