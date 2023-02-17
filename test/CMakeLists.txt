# Some compilers warn that the return value of cr_assert gets converted from an
# int to a unsigned char:1. To avoid this error in those compilers, turn off
# conversion warnings for test code.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-conversion")

add_executable(test_hello test_hello.c)
target_link_libraries(test_hello
  PRIVATE hello 
  PUBLIC ${CRITERION}
)
add_test(
  NAME test_hello
  COMMAND test_hello ${CRITERION_FLAGS}
)

add_executable(test_vector test_vector.c)
target_link_libraries(test_vector
  PUBLIC ${CRITERION}
)
add_test(
  NAME test_vector
  COMMAND test_vector ${CRITERION_FLAGS}
)


