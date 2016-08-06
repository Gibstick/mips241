find_package(Sanitizers)


add_executable(test_decode test_decode.c)
target_link_libraries(test_decode mips241)
add_sanitizers(test_decode)

add_test(NAME decode-unit-test COMMAND "$<TARGET_FILE:test_decode>")

add_executable(test_disassemble test_disassemble.c)
target_link_libraries(test_disassemble mips241)
add_sanitizers(test_disassemble)

add_test(NAME disassemble-unit-test COMMAND "$<TARGET_FILE:test_disassemble>")
