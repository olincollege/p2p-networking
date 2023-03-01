# Require the CMake version matching what is included in the latest LTS version
# of Ubuntu, which at the time of writing is 22.04.
cmake_minimum_required(VERSION 3.22)

project(SoftSysA02
  VERSION 1.0
  LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,stack-size=10000000000000")
set(CMAKE_C_FLAGS "-isystem/usr/local/Cellar/criterion/2.4.1_1/include -Wall -Wextra -pedantic -Wshadow -Wconversion -Wunreachable-code -g")

set(CMAKE_C_CLANG_TIDY
  clang-tidy;
  -header-filter=.;
  -checks=*,-bugprone-easily-swappable-parameters,-llvmlibc-restrict-system-libc-headers,-llvm-header-guard,-altera*;
)

add_subdirectory(src)

include(CTest)
find_library(CRITERION criterion REQUIRED)
set(CRITERION_FLAGS --verbose -j1)

add_subdirectory(test)
