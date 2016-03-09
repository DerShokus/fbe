find_program(RAGEL "ragel")

function(ragel_gen in_rl)
    add_custom_command(
        OUTPUT ${in_rl}.c
        COMMAND ${RAGEL} -o ${in_rl}.c ${in_rl}.rl -I ${CMAKE_CURRENT_SOURCE_DIR} ${ARGN}
        DEPENDS ${in_rl}.rl
        )
    add_custom_target(ragel_${in_rl} DEPENDS ${in_rl}.c)
endfunction(ragel_gen)

if(RAGEL)
    message(STATUS "ragel found at: ${RAGEL}")
else(RAGEL)
    message(FATAL_ERROR "ragel not found")
endif(RAGEL)
