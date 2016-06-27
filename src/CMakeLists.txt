include_directories(".")

set(CMAKE_POSITION_INDEPENDENT_CODE on)

add_subdirectory("machine")
add_subdirectory("emulator")
add_subdirectory("frontend")

add_library(mips241 SHARED $<TARGET_OBJECTS:machine> $<TARGET_OBJECTS:emulator>)
