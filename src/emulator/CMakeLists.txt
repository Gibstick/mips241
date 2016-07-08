add_library(emulator OBJECT emulator.c)

add_executable(cmips241 main.c)

target_link_libraries(cmips241 mips241)
