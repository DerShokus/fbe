include_directories(../include ../lib/trie/include)

link_directories("${CMAKE_BINARY_DIR}/lib/trie/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

ragel_gen(lexer -L)
lemon_gen(grammar)

add_library(fbe ast.c
                command.c
                fbe.c
                grammar.c
                heap.c
                lexer.c
                oparg.c
                opcode.c)

target_link_libraries(fbe LINK_PUBLIC trie)

set_target_properties(fbe
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../bin"
)
