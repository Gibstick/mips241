find_program(SH sh)

if (SH)
    add_test(
        NAME "basic-test" 
        COMMAND ${SH} ${CMAKE_CURRENT_SOURCE_DIR}/runtest.sh "$<TARGET_FILE:mips241>"
        ${CMAKE_CURRENT_SOURCE_DIR}/basic/basic.config
    )
endif (SH)

add_subdirectory(unit)
