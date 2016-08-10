find_program(RACKET racket
    PATHS /usr/racket/bin)
if (NOT RACKET)
    message(FATAL_ERROR "Racket was not found.")
endif()

if ($ENV{RACKET_FAST_BUILD})
    set(RACKET_FAST_BUILD "on")
endif()

if (RACKET_FAST_BUILD)
    SET(RACO_EXE_FLAGS ${RACO_EXE_FLAGS} -l)
    message(STATUS "Using Racket launchers instead of executables.")
endif()

add_custom_target(copy_racket_frontends ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/racket"
)

add_custom_target(racket_frontends ALL
    DEPENDS copy_racket_frontends
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/racket"
    COMMAND ${RACKET} -l raco/main make bindings.rkt frontend.rkt custom.rkt
    COMMAND ${RACKET} -l raco/main exe ${RACO_EXE_FLAGS} -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mips241.basic" frontend.rkt
    COMMAND ${RACKET} -l raco/main exe ${RACO_EXE_FLAGS} -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mips241.custom" custom.rkt
    COMMAND ${RACKET} -l raco/main exe ${RACO_EXE_FLAGS} -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mips241.legacy" legacy.rkt
    COMMENT "Compiling Racket frontends"
)

add_test(
    NAME guess-file-type
    COMMAND ${RACKET} -l raco/main test "${CMAKE_CURRENT_SOURCE_DIR}/guess-file-type.rkt"
)
