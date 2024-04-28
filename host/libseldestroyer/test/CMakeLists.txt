
add_executable(test_open_close_c c/open_close.c)
add_executable(test_wrong_device cpp/wrong_device.cpp)
target_link_libraries(test_wrong_device seldestroyer)
target_link_libraries(test_open_close_c seldestroyer)

configure_file(exec_tests.sh exec_tests.sh COPYONLY)
