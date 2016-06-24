find_program(SH sh)

if (SH)
    add_test(
        NAME "smoke-test" 
        COMMAND ${SH} ${CMAKE_CURRENT_SOURCE_DIR}/runtest.sh "$<TARGET_FILE:mips241>"
        ${CMAKE_CURRENT_SOURCE_DIR}/basic/smoke.config
    )
endif (SH)

add_subdirectory(unit)
