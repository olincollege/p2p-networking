# Some compilers warn that the return value of cr_assert gets converted from an
# int to a unsigned char:1. To avoid this error in those compilers, turn off
# conversion warnings for test code.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-conversion")

add_executable(test_hash_table test_hash_table.c)
target_link_libraries(test_hash_table
  PRIVATE include_all 
  PUBLIC ${CRITERION}
)
add_test(
  NAME test_hash_table
  COMMAND test_hash_table ${CRITERION_FLAGS}
)

add_executable(test_vector test_vector.c)
target_link_libraries(test_vector
  PRIVATE include_all
  PUBLIC ${CRITERION}
)
add_test(
  NAME test_vector
  COMMAND test_vector ${CRITERION_FLAGS}

)
add_executable(test_message test_message.c)
target_link_libraries(test_message
  PRIVATE include_all
  PUBLIC ${CRITERION}
)
add_test(
  NAME test_message 
  COMMAND test_message ${CRITERION_FLAGS}
)

add_executable(test_network test_network.c)
target_link_libraries(test_network
  PRIVATE include_all
  PUBLIC ${CRITERION}
)
add_test(
  NAME test_network
  COMMAND test_network ${CRITERION_FLAGS}
)

add_executable(test_state test_state.c)
target_link_libraries(test_state
  PRIVATE include_all ssl crypto
  PUBLIC ${CRITERION}
)
add_test(
  NAME test_state
  COMMAND test_state ${CRITERION_FLAGS}
)

target_link_libraries(loop
    PRIVATE include_all
)
