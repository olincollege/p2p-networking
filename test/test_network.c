#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <stdlib.h>
#include <unistd.h>

#include "../src/network.h"

// NOLINTBEGIN -- the magic constant warnings are useless here
Test(test_socket, check_expand_socket_buffer) {
  // expanding buffer size requires sudo
  // only run this test on CI because of annoyance of using sudo on tests
  // locally
  if (getenv("CI")) {
    // make sure that we can expand socket buffer past 1MB

    // get a new socket
    int sock = create_socket(0).file_descriptor;

    // look at the default buffer size
    int original_rec_buffer = 0;
    unsigned int int_size = sizeof(original_rec_buffer);
    getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&original_rec_buffer,
               &int_size);

    // increase the buffer size
    large_buffer_socket(sock);

    // look at the new buffer size
    int EXPECTED_BUFFER_SIZE = 1024 * 1024 * 100; // 100 Mib
    int changed_rec_buffer = 0;
    int changed_send_buffer = 0;
    getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&changed_rec_buffer,
               &int_size);
    getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&changed_send_buffer,
               &int_size);

    cr_assert(eq(int, 1, changed_rec_buffer > original_rec_buffer));
    cr_assert(eq(int, 2 * EXPECTED_BUFFER_SIZE, changed_rec_buffer));
    cr_assert(eq(int, 2 * EXPECTED_BUFFER_SIZE, changed_send_buffer));

    // closing socket should pass without error
    cr_assert(eq(int, 0, close(sock)));
  } else {
    cr_skip("skipping test because not on CI");
  }
}
// NOLINTEND
