#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/message.h"

// NOLINTBEGIN -- the magic constant warnings are useless here
Test(message, assert_message) {
    // make sure that no weird things are happening with struct alignment or repacking by the compiler 
    _Static_assert (sizeof(peer_info) == 18, "peer info should be 18 bytes"); 
    _Static_assert (sizeof(ask_message) == 21, "ask_message should be 21 bytes"); 
    _Static_assert (sizeof(give_message) == 131093, "give message should be 131093 bytes"); 
}
// NOLINTEND
