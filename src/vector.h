#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma GCC diagnostic ignored "-Wunused-function"

// NOLINTBEGIN -- false warnings from strict-prototypes and macro () enclosure
// also memcpy warning is non-relevant
#define vector__(TYPE, NAME)                                                   \
  /* A dynamic vector of type #TYPE akin to std::vector from c++ */            \
  typedef struct vector_##NAME {                                               \
    TYPE *arr;                                                                 \
    size_t size;                                                               \
    size_t alloc;                                                              \
  } vector_##NAME;                                                             \
                                                                               \
  /* Inserts an element to the end of the vector */                            \
  static void push_vec_##NAME(vector_##NAME *vec, TYPE element) {              \
    if (vec->size == vec->alloc) {                                             \
      TYPE *new_arr = malloc(sizeof(TYPE) * vec->alloc * 2);                   \
      memcpy(new_arr, vec->arr, sizeof(TYPE) * vec->alloc);                    \
      free(vec->arr);                                                          \
      vec->arr = new_arr;                                                      \
      vec->alloc *= 2;                                                         \
    }                                                                          \
    vec->size++;                                                               \
    vec->arr[vec->size - 1] = element;                                         \
  }                                                                            \
  /* Returns a pointer to the alement at a given index in the vector */        \
  static TYPE *get_vec_##NAME(vector_##NAME *vec, size_t idx) {                \
    return &vec->arr[idx];                                                     \
  }                                                                            \
  /* Creates a vector container with no elements in it */                      \
  static vector_##NAME new_vec_##NAME() {                                      \
    vector_##NAME new_struct = {malloc(sizeof(TYPE)), 0, 1};                   \
    return new_struct;                                                         \
  }                                                                            \
  /* De-allocates all dynamic memory used by the container */                  \
  static void free_vec_##NAME(vector_##NAME *vec) {                            \
    free(vec->arr);                                                            \
    vec->arr = NULL;                                                           \
  }                                                                            \
  // NOLINTEND
