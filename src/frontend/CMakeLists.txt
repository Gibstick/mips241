find_program(RACKET racket
    PATHS /usr/racket/bin)

if (NOT RACKET)
    message(FATAL_ERROR "Racket was not found.")
endif()

add_custom_target(copy_racket_frontends ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "{CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/racket"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/bindings.rkt" "${CMAKE_BINARY_DIR}/racket/bindings.rkt"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/frontend.rkt" "${CMAKE_BINARY_DIR}/racket/frontend.rkt"
)

add_custom_target(racket_basic_frontend ALL
    DEPENDS copy_racket_frontends
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/racket"
    COMMAND ${RACKET} -l raco/main make bindings.rkt frontend.rkt
    COMMAND ${RACKET} -l raco/main exe -o "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mips241.basic" frontend.rkt
    COMMENT "Compiling Racket frontends"
)
