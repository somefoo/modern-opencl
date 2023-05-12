fs = import('fs')

# Create quick_test executable
quick_test = executable('quick_test', 'quick_test.cpp',
                        dependencies : modern_opencl_dep,
                        #include_directories : [modern_opencl_lib.include_directories()],
                        # Define KERNEL_DIR macro with path to test kernels
                        cpp_args : '-DKERNEL_DIR="@0@"'.format(meson.current_source_dir() + '/test_kernels/'))

# Add quick_test as a test
test('Quick', quick_test)

# Create quick_test executable
quick_precompiled_test = executable('quick_precompiled_test', 'quick_precompiled_test.cpp',
                        dependencies : modern_opencl_dep,
                        #include_directories : [modern_opencl_lib.include_directories()],
                        # Define KERNEL_DIR macro with path to test kernels
                        cpp_args : '-DKERNEL_DIR="@0@"'.format(meson.current_source_dir() + '/test_kernels/'))

# Add quick_test as a test
test('QuickPrecompiled', quick_precompiled_test)
