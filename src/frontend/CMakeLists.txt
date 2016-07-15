find_program(RACKET racket
    PATHS /usr/racket/bin)
if (NOT RACKET)
    message(FATAL_ERROR "Racket was not found.")
endif()

add_custom_target(copy_racket_frontends ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "{CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/racket"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/bindings.rkt" "${CMAKE_BINARY_DIR}/racket/bindings.rkt"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/frontend.rkt" "${CMAKE_BINARY_DIR}/racket/frontend.rkt"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/custom.rkt" "${CMAKE_BINARY_DIR}/racket/custom.rkt"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/legacy.rkt" "${CMAKE_BINARY_DIR}/racket/legacy.rkt"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/guess-file-type.rkt" "${CMAKE_BINARY_DIR}/racket/guess-file-type.rkt"
)

add_custom_target(racket_frontends ALL
    DEPENDS copy_racket_frontends
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/racket"
    COMMAND ${RACKET} -l raco/main make bindings.rkt frontend.rkt custom.rkt
    COMMAND ${RACKET} -l raco/main exe -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mips241.basic" frontend.rkt
    COMMAND ${RACKET} -l raco/main exe -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mips241.custom" ++lib racket/lang/reader custom.rkt
    COMMAND ${RACKET} -l raco/main exe -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mips241.legacy" legacy.rkt
    COMMENT "Compiling Racket frontends"
)

add_test(
    NAME guess-file-type
    COMMAND ${RACKET} -l raco/main test "${CMAKE_CURRENT_SOURCE_DIR}/guess-file-type.rkt"
)
