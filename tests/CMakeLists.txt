include_directories(../include)

link_directories("${CMAKE_CURRENT_BINARY_DIR}/bin")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-logical-op-parentheses")

add_executable(eval eval.c)
add_executable(args args.c)
add_executable(graph graph.c)

target_link_libraries(eval LINK_PUBLIC fbe)
target_link_libraries(args LINK_PUBLIC fbe)
target_link_libraries(graph LINK_PUBLIC fbe)


set_target_properties(eval args graph
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
)
