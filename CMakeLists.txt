cmake_minimum_required (VERSION 2.8.11)

set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

include(cmake/ragel.cmake)
include(cmake/lemon.cmake)

# Builds
add_subdirectory (lib/trie)
add_subdirectory (src)

# Tests
add_subdirectory (tests)

# CTest
enable_testing()
add_test (NAME BruteForce COMMAND ./tests/bin/eval)
