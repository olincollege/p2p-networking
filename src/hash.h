#pragma once

#include <string.h>

/**
 * Hash string with the djb2 algorith.
 *
 * http://www.cse.yorku.ca/~oz/hash.html
 * A copy of the djb2 hash function by
 * https://en.wikipedia.org/wiki/Daniel_J._Bernstein It is not cryptographically
 * secure, but is effective and fast in practice.
 *
 * @param str A null terminated string to hash.
 * @return Result of the hashing algorithm.
 */
unsigned long djb2(unsigned char *str);

/**
 * Hash binary data with the djb2 algorithm.
 *
 * @param bytes The location of binary data to hash
 * @param num The number of bytes to hash, (sizeof)
 * @return Result of the hashing algorithm.
 */
unsigned long djb2_bytes(void *bytes, size_t num);
