#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/vector.h"

// The magic constant warnings are useless here
// NOLINTBEGIN
Test(vector, vector_int) {
    // add elements to container
    vector_int vec = new_vec_int();
    push_vec_int(&vec, 20);
    push_vec_int(&vec, 4); 
    push_vec_int(&vec, 3); 
    push_vec_int(&vec, 8); 
    push_vec_int(&vec, -1000); 

    // elements are in the container correectly
    cr_assert(eq(int, *get_vec_int(&vec, 0), 20));
    cr_assert(eq(int, *get_vec_int(&vec, 1), 4));
    cr_assert(eq(int, *get_vec_int(&vec, 2), 3));
    cr_assert(eq(int, *get_vec_int(&vec, 3), 8));
    cr_assert(eq(int, *get_vec_int(&vec, 4), -1000));

    // container size correct
    cr_assert(eq(int, vec.size, 5));

    // de-allocate container
    free_vec_int(&vec);

}
// NOLINTEND
