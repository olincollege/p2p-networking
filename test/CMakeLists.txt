# Some compilers warn that the return value of cr_assert gets converted from an
# int to a unsigned char:1. To avoid this error in those compilers, turn off
# conversion warnings for test code.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-conversion")

add_executable(test_hash_table test_hash_table.c)
target_link_libraries(test_hash_table
  PRIVATE hash_table 
  PUBLIC ${CRITERION}
)
add_test(
  NAME test_hash_table
  COMMAND test_hash_table ${CRITERION_FLAGS}
)

add_executable(test_vector test_vector.c)
target_link_libraries(test_vector
  PUBLIC ${CRITERION}
)
add_test(
  NAME test_vector
  COMMAND test_vector ${CRITERION_FLAGS}
)


