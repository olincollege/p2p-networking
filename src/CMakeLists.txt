add_library(
    include_all
    hash_table.h 
    hash_table.c 
    hash.h 
    hash.c
    vector.h
    network.h
    network.c
    message.h
    state.c
    state.h
)

target_link_libraries(include_all ssl crypto)
add_executable(loop ../src/loop.c)
