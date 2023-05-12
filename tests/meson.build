fs = import('fs')
# Create quick_test executable
quick_test = executable('quick_test', 'quick_test.cpp',
                        dependencies : modern_opencl_dep,
                        #link_with : [modern_opencl_lib],
                        #include_directories : [modern_opencl_lib.include_directories()],
                        # Define KERNEL_DIR macro with path to test kernels
                        cpp_args : '-DKERNEL_DIR="@0@"'.format(meson.current_source_dir() + '/test_kernels/'))

# Add quick_test as a test
test('Quick', quick_test)

# Path to kernel files
kernel_path = meson.current_source_dir() + '/test_kernels/'
#kernel_path = meson.current_source_dir() + '/test_kernels/quick_precompiled_test.cl'

# Get all files in kernel_path
kernel_files = files(kernel_path + 'quick_precompiled_test.cl', kernel_path + 'quick_test.cl')
kernel_files_string = ''

opencl_dep = dependency('OpenCL')
opencl_headers = opencl_dep.get_pkgconfig_variable('includedir')

message('@0@'.format(opencl_headers))

list_of_kernels = []
# Print all files in kernel_path
foreach file : kernel_files
  message('@0@'.format(file))
  kernel_name = fs.stem(file)
  kernel = custom_target(kernel_name,
                         output : kernel_name + '.spv',
                         input : file,
                         command : ['clang', '-cl-std=CL2.0', '-c', '-target', 'spir64', '-O0', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         build_by_default : true)
  list_of_kernels += kernel

endforeach
message('@0@'.format(list_of_kernels))
message('@0@'.format(kernel_files_string))

kernel_header = custom_target('kernels.hpp',
                              output : 'kernels.hpp',
                              input : list_of_kernels,
                              command : [prog_python, spirv_to_header_conversion_tool, '@INPUT@', '@OUTPUT@'],
                              build_by_default : true)
