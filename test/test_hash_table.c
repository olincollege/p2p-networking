#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/hash_table.h"

// NOLINTBEGIN -- the magic constant warnings are useless here

// Test that we can create a hash table, put in 2 values, and take out those
// values.
Test(test_map, simple_test) {
    hash_table table = make_table();
    // char key1[] = {'k', 'e', 'y', '1'};
    // char key2[] = {'k', 'e', 'y', '2'};
    int key1 = 1; // We can now hash literally anything
    int key2 = 2;
    int val1 = 100;
    int val2 = 200;
    set_value(&table, &key1, sizeof(key1), &val1, sizeof(val1));
    set_value(&table, &key2, sizeof(key2), &val2, sizeof(val2));

    kv_pair* kv1 = get_kv_pair(&table, &key1, sizeof(key1)); 
    int res1 = *(int*)kv1->value;
    cr_assert(eq(int, val1, res1));

    kv_pair* kv2 = get_kv_pair(&table, &key2, sizeof(key2)); 
    int res2 = *(int*)kv2->value;
    cr_assert(eq(int, val2, res2));

    hash_dealloc(&table);
}

// the presence of a null-terminator char should not affect anything
Test(test_map, null_terminated_data) {
    hash_table table = make_table();
    for(size_t total_iter = 0; total_iter < 26; total_iter++) {
        char keyx[3] = {'\0', 'a'+total_iter%26, 'b'};
        int datax = total_iter;
        set_value(&table, keyx, sizeof(char)*3, &datax, sizeof(datax));
    }
    char key1[3] = {'\0', 'a', 'b'};
    char key2[3] = {'\0', 'b', 'b'};

    kv_pair* kv1 = get_kv_pair(&table, &key1, sizeof(char)*3); 
    kv_pair* kv2 = get_kv_pair(&table, &key2, sizeof(char)*3); 

    int res1 = *(int*)kv1->value;
    int res2 = *(int*)kv2->value;

    cr_assert(eq(int, res1, 0));
    cr_assert(eq(int, res2, 1));

    hash_dealloc(&table);

}

// test removing and collecting elements 
Test(test_map, remove_collect_elements) {
    hash_table table = make_table(); 
    char key1[] = {'\0', 'a', 'b', 'c', 'd'};
    char data1[] = {'a', 'b', 'c', 'd', 'e'};
    
    // hash_table is empty at the start
    cr_assert(eq(int, 0, table.num_elements));

    // hash table has one element now
    set_value(&table, key1, sizeof(key1), data1, sizeof(data1));
    cr_assert(eq(int, 1, table.num_elements));
    
    // table should still have one element
    set_value(&table, key1, sizeof(key1), data1, sizeof(data1));
    cr_assert(eq(int, 1, table.num_elements));

    // collect should have one element
    vector_kv_pair elements = collect_table(&table);
    cr_assert(eq(int, 1, elements.size));
    cr_assert(eq(int, 0, memcmp(elements.arr[0].value, &data1, sizeof(data1))));

    // hash table should be empty now
    remove_kv_pair(&table, key1, sizeof(key1));
    cr_assert(eq(int, 0, table.num_elements));


    // table should be empty



    hash_dealloc(&table);
}

// Tested with valgrind against memory leaks
Test(test_map, advance_test) {
    for(size_t total_iter = 0; total_iter < 1000; total_iter++) {
        size_t iters = 26;
        hash_table table = make_table();
        for(size_t idx = 0; idx < iters; idx++) {
            char key[4] = "abc";
            key[3] = 'a' + idx;
            key[4] = '\0';
            set_value(&table, key, strlen(key)+1, &idx, sizeof(idx));
        }
        for(size_t idx = 0; idx < iters; idx++) {
            char key[4] = "abc";
            key[3] = 'a' + idx;
            key[4] = '\0';
            kv_pair* kv = get_kv_pair(&table, key, strlen(key)+1); 
            int actual_value = *(int*)kv->value;
            cr_assert(eq(int, idx, actual_value));
        }
        hash_dealloc(&table);
    }
}


// Tested with valgrind against memory leaks
Test(test_map, memory_leak_test) {
    for(size_t iter = 0; iter < 10000; iter++) {
        hash_table table = make_table();
        char* key1 = "key1";
        char* key2 = "key2";
        int val1 = 1;
        int val2 = 2;
        set_value(&table, key1, strlen(key1)+1, &val1, sizeof(val1));
        set_value(&table, key2, strlen(key2)+1, &val2, sizeof(val2));

        kv_pair* kv1 = get_kv_pair(&table, key1, strlen(key1)+1); 
        int res1 = *(int*)kv1->value;
        cr_assert(eq(int, val1, res1));

        kv_pair* kv2 = get_kv_pair(&table, key2, strlen(key2)+1); 
        int res2 = *(int*)kv2->value;
        cr_assert(eq(int, val2, res2));

        hash_dealloc(&table);
    }
}



// NOLINTEND
