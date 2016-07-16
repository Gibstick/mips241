find_program(SH sh)

if (SH)
    #    add_test(
    #        NAME "basic-test" 
    #        COMMAND ${SH} ${CMAKE_CURRENT_SOURCE_DIR}/runtest.sh "$<TARGET_FILE:cmips241>"
    #        ${CMAKE_CURRENT_SOURCE_DIR}/basic/basic.config
    #    )

    add_test(
        NAME "racket-basic-test"
        COMMAND ${SH} ${CMAKE_CURRENT_SOURCE_DIR}/runtest.sh
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mips241.legacy
        ${CMAKE_CURRENT_SOURCE_DIR}/basic/basic.config
        racket
    )

    add_test(
        NAME "auto-assemble-test"
        COMMAND ${SH} ${CMAKE_CURRENT_SOURCE_DIR}/runtest.sh
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mips241.legacy
        ${CMAKE_CURRENT_SOURCE_DIR}/ascii/ascii.config
        racket
    )

endif()

add_subdirectory(unit)
