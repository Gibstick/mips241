add_library(emulator OBJECT emulator.c)

add_executable(mips241-bare main.c)

target_link_libraries(mips241-bare mips241)
