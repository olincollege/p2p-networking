#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/hash_table.h"

// NOLINTBEGIN -- the magic constant warnings are useless here
Test(test_map, simple_test) {
    hash_table table = make_table();
    const* key1 = "key1";
    const* key2 = "key2";
    int val1 = 1;
    int val2 = 2;
    set_value(&table, key1, &val1, sizeof(val1));
    set_value(&table, key2, &val2, sizeof(val2));

    kv_pair* kv1 = get_kv_pair(&table, key1); 
    int res1 = *(int*)kv1->value;
    cr_assert(eq(int, val1, res1));

    kv_pair* kv2 = get_kv_pair(&table, key2); 
    int res2 = *(int*)kv2->value;
    cr_assert(eq(int, val2, res2));
}

Test(test_map, memory_leak_test) {
    for(size_t iter = 0; iter < 100000000; iter++) {
        hash_table table = make_table();
        const* key1 = "key1";
        const* key2 = "key2";
        int val1 = 1;
        int val2 = 2;
        set_value(&table, key1, &val1, sizeof(val1));
        set_value(&table, key2, &val2, sizeof(val2));

        kv_pair* kv1 = get_kv_pair(&table, key1); 
        int res1 = *(int*)kv1->value;
        cr_assert(eq(int, val1, res1));

        kv_pair* kv2 = get_kv_pair(&table, key2); 
        int res2 = *(int*)kv2->value;
        cr_assert(eq(int, val2, res2));
    }
}


// NOLINTEND
