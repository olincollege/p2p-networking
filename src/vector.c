#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

#define __VECTOR(TYPE, NAME)                                              \
                                                                          \
    /* A dynamic vector of type #TYPE akin to std::vector from c++ */     \
    typedef struct vector_##NAME {                                        \
        TYPE* arr;                                                        \
        size_t size;                                                      \
        size_t alloc;                                                     \
    } vector_##NAME;                                                      \
                                                                          \
    /* Inserts an element to the end of the vector */                     \
    void push_vec_##NAME(vector_##NAME* vec, TYPE element) {              \
        if(vec->size == vec->alloc) {                                     \
            TYPE* new_arr = malloc(sizeof(TYPE) * vec->alloc * 2);        \
            memcpy(new_arr, vec->arr, sizeof(TYPE) * vec->alloc * 2);     \
            free(vec->arr);                                               \
            vec->arr = new_arr;                                           \
            vec->alloc *= 2;                                              \
        }                                                                 \
        vec->size ++;                                                     \
        vec->arr[vec->size-1] = element;                                  \
    }                                                                     \
    /* Returns a pointer to the alement at a given index in the vecotr */ \
    TYPE* get_vec_##NAME(vector_##NAME* vec, size_t idx) {                \
        return &vec->arr[idx];                                            \
    }                                                                     \
    /* Creates a vector container with no elements in it */               \
    vector_##NAME new_vec_##NAME() {                                      \
        vector_##NAME new_struct = {malloc(sizeof(TYPE)), 0, 1};          \
        return new_struct;                                                \
    }                                                                     \
    /* De-allocates all dynamic memory used by the container */           \
    void free_vec_##NAME(vector_##NAME *vec) {                            \
        free(vec->arr);                                                   \
        vec->arr = NULL;                                                  \
    }                                                                     \


// define common vector types we might need
__VECTOR(int, int);
__VECTOR(char, char);
__VECTOR(int, intptr);
__VECTOR(char*, charptr);

int main() {
    vector_int vec = new_vec_int();
    push_vec_int(&vec, 20);
    push_vec_int(&vec, 4); 
    push_vec_int(&vec, 3); 
    push_vec_int(&vec, 8); 
    
    printf("%d\n", *get_vec_int(&vec, 3));

    puts("hello"); 
}
