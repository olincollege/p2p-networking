#include "./vector.h"
#include <stdio.h>
#include "./hash.h"
#include "./hash_table.h"
#include <string.h>

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

/* Internal, hash table with custom initial bucket size */
hash_table __make_table(size_t bucket_size) {
    hash_table new_table = {malloc(sizeof(vector_kv_pair) * bucket_size), bucket_size, 0};
    return new_table;
}
/* Creates a new empty hash table */
hash_table make_table() {
    return __make_table(2);
}

/* Returns a key/value pair in the hash_table or a NULL padded struct if it does not exist */
kv_pair* get_kv_pair(hash_table* in_table, char* key) {
    // find the bucket for the key
    size_t bucket = djb2((unsigned char*)key) % in_table->bucket_size;    
    // walk the bucket
    for(size_t index = 0; index < (in_table->buckets[bucket].size); index++) {
        char* ret_key = in_table->buckets[bucket].arr[index].key; 
        if(strcmp(key, ret_key) == 0) {
            return &in_table->buckets[bucket].arr[index];
        }
    }
    return NULL;
}

/* Frees the data used by the hash table */
void hash_dealloc(hash_table* in_table) {

    free(in_table->buckets);
    in_table->buckets = NULL;
}

void hash_realloc(hash_table* in_table);
void set_value(hash_table* in_table, char* key, void* value, size_t value_size);

/* Reallocs the hash table to handle more elements */
void hash_realloc(hash_table* in_table) {
    // allocate a new table with double the bucket size
    hash_table new_table = __make_table(in_table->bucket_size * 2);
    // copy elements in
    for(size_t bucket = 0; bucket < in_table->bucket_size; bucket ++) {
        for(size_t element = 0; element < in_table->buckets[element].size; element++) {
            kv_pair cur_pair = in_table->buckets[bucket].arr[element]; 
            set_value(&new_table, cur_pair.key, cur_pair.value, cur_pair.value_size);
        }
    }
    // swap new table with input table
    hash_dealloc(in_table);
    in_table->buckets = new_table.buckets; 
    in_table->bucket_size *= 2;
}

/* Updates the value of a key in a hash table */
void set_value(hash_table* in_table, char* key, void* value, size_t value_size) {
    kv_pair* get_pair = get_kv_pair(in_table, key);     
    if(get_pair) {
        free(get_pair->value); 
        get_pair->value = malloc(value_size); 
        memcpy(value, get_pair->value, value_size); // NOLINT
    }
    else {
        puts("not found");        
        if(in_table->num_elements == in_table->bucket_size) {
            puts("realloced");
            hash_realloc(in_table);
        }
        in_table->num_elements ++;
        size_t bucket = djb2((unsigned char*)key) % in_table->bucket_size; 
        printf("%d\n", (int)bucket);
        kv_pair new_pair = {malloc(strlen(key)+1), malloc(value_size), value_size};
        memcpy(new_pair.key, key, strlen(key)+1); // NOLINT
        memcpy(new_pair.value, value, value_size); // NOLINT
        push_vec_kv_pair(&in_table->buckets[bucket], new_pair);
    }
}

int main() {
   hash_table table = make_table(); 
   char* key = "foobar";
   int val = 10;
   set_value(&table, key, &val, 1);
   kv_pair* kv = get_kv_pair(&table, key);
   printf("%d\n", *(int*) kv->value);

}
