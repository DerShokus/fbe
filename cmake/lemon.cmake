find_program(LEMON "lemon")

function(lemon_gen in_lemon)
    add_custom_command(
        OUTPUT ${in_lemon}.c
        COMMAND ${LEMON} ${in_lemon}.y
        DEPENDS ${in_lemon}.y
        )
    add_custom_target(lemon_${in_lemon} DEPENDS ${in_lemon}.c)
endfunction(lemon_gen)

if(LEMON)
    message(STATUS "lemon found at: ${RAGEL}")
else(LEMON)
    message(FATAL_ERROR "lemon not found")
endif(LEMON)

