message(STATUS "Building tests...")

#add_definitions(quick_test -DKERNEL_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_kernels")
add_executable(quick_test quick_test.cpp)
target_compile_definitions(quick_test PRIVATE KERNEL_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_kernels/")
target_link_libraries(quick_test PRIVATE modern-opencl)
add_test(Quick quick_test)
