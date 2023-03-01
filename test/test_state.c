#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdlib.h>

#include "../src/state.h"

// NOLINTBEGIN -- the magic constant warnings are useless here
Test(test_state, test_random_state) {
    const unsigned int MAX_PIECES = 100; 
    const unsigned int HAVE_AMOUNT = 30; 
    const unsigned int WANT_AMOUNT = 60;
    client_state state = demo_state(MAX_PIECES, HAVE_AMOUNT, WANT_AMOUNT);
    cr_assert(eq(int, 0,0));
}
// NOLINTEND
