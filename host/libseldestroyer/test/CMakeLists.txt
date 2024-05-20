
add_executable(test_open_close_c c/open_close.c)
add_executable(test_wrong_device cpp/wrong_device.cpp)
add_executable(test_full_test_ok cpp/full_test_ok.cpp)
target_link_libraries(test_wrong_device seldestroyer)
target_link_libraries(test_open_close_c seldestroyer)
target_link_libraries(test_full_test_ok seldestroyer)

configure_file(exec_tests.sh exec_tests.sh COPYONLY)
