#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "../src/hello.h"

Test(strip, empty_string) {
  cr_assert(eq(str, hello(), "hello"));
}

