project('ModernOpenCL', 'cpp', default_options : ['default_library=static', 'cpp_std=c++20'])


if meson.get_compiler('cpp').get_id() != 'clang'
	warning('ModernOpenCL requires Clang (to compile OpenCL kernels to SPIR-V).')
	message('You can use Clang by setting the CC and CXX environment variables to clang and clang++ respectively.')
	error('Wrong compiler')
endif

# Get shader_path from options and check if it empty
opencl_kernel_folder_path = get_option('opencl_kernel_folder_path')

if opencl_kernel_folder_path == ''
  warning('''
      You have not set the opencl_kernel_folder_path option in the file: @0@/meson.options
      Your custom OpenCL kernels will not be compiled!

      Building tests instead!
      '''.format(meson.current_source_dir()))
  opencl_kernel_folder_path = get_option('opencl_kernel_folder_path_test')
else
  message('Folder with kernel was set in options: @0@'.format(opencl_kernel_folder_path))
endif


opencl_dep = dependency('OpenCL')
#modern_opencl_lib = static_library('modern-opencl')

compile_args = ['-Og', '-Wall', '-Wextra']





##############################################
# Compile OpenCL kernels to SPIR-V
fs = import('fs')

prog_python = find_program('python3')
spirv_to_header_conversion_tool = '@0@'.format(meson.current_source_dir()) + '/utility/multiple_spirv_to_cpp.py'



# Get all files in opencl_kernel_folder_path
kernel_files = files(opencl_kernel_folder_path + '/quick_precompiled_test.cl', opencl_kernel_folder_path + '/quick_test.cl')
kernel_files_string = ''

opencl_dep = dependency('OpenCL')
opencl_headers = opencl_dep.get_pkgconfig_variable('includedir')

message('@0@'.format(opencl_headers))

kernel_data_folder = 'generated_llvm_and_spirv_files/'

list_of_kernels = []
# Print all files in opencl_kernel_folder_path
foreach file : kernel_files
  message('@0@'.format(file))
  kernel_name = fs.stem(file)
  kernel_llvm = custom_target(kernel_name + '_llvmspv',
                         output : kernel_name + '.llvmspv',
                         input : file,
                         command : ['clang', '-cl-std=CL2.0', '-c', '-target', 'spir64', '-O0', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         build_by_default : true,
                         )

  kernel_spv = custom_target(kernel_name,
                          output : kernel_name + '.spv',
                          input : kernel_llvm,
                          command : ['llvm-spirv', '@INPUT@', '-o', '@OUTPUT@'],
                          build_by_default : true)
  list_of_kernels += kernel_spv
endforeach

kernel_compilation_cpp = custom_target('clw_generated_kernels.cpp',
                              output : 'clw_generated_kernels.cpp',
                              input : list_of_kernels,
                              command : [prog_python, spirv_to_header_conversion_tool, '@INPUT@', '@OUTPUT@'],
                              build_by_default : true)

# Compile object
#kernel_object = library('clw_generated_kernels', kernel_compilation_object)

sources = [
  'src/clw_context.cpp',
  kernel_compilation_cpp,
]

include_dirs = [
  'include/',
]

dependencies = [
  opencl_dep,
]

modern_opencl_lib = static_library('modern-opencl', sources, include_directories : include_dirs, dependencies : dependencies, c_args : compile_args, cpp_args : compile_args, pic : true)

link_with = [
  modern_opencl_lib,
]

modern_opencl_dep = declare_dependency(version : '1.0', include_directories : include_dirs, dependencies : dependencies, link_with : link_with)

subdir('tests')
