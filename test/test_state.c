#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <stdlib.h>

#include "../src/state.h"

// NOLINTBEGIN -- the magic constant warnings are useless here
Test(test_state, test_random_state) {
    const unsigned int MAX_PIECES = 100; 
    const unsigned int HAVE_AMOUNT = 30; 
    const unsigned int WANT_AMOUNT = 60;
    client_state state = demo_state(MAX_PIECES, HAVE_AMOUNT, WANT_AMOUNT);

    cr_assert(eq(int, HAVE_AMOUNT, state.pieces_have.num_elements));
    cr_assert(eq(int, WANT_AMOUNT, state.pieces_want.num_elements));
}

Test(test_state, test_peer_exhange, .init = cr_redirect_stdout) {
   FILE* f_stdin = cr_get_redirected_stdin(); 
   int fin_fd = fileno(f_stdin);

    // say we know about 4 random ports
   client_state state = new_state();
   add_port(&state, 1);
   add_port(&state, 2);
   add_port(&state, 3);
   add_port(&state, 4);
}

// NOLINTEND
