find_package(Sanitizers)


add_executable(test_decode test_decode.c)
target_link_libraries(test_decode machine)
add_sanitizers(test_decode)

add_test(NAME decode-unit-test COMMAND "$<TARGET_FILE:test_decode>")
