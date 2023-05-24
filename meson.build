project('ModernOpenCL', 'cpp', default_options : ['default_library=static', 'cpp_std=c++20'])

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

# Tool used to convert multiple spirv or ptx files to a single header file
spirv_to_header_conversion_tool = '@0@'.format(meson.current_source_dir()) + '/utility/multiple_spirv_to_cpp.py'

# Tool used to allow globbing in meson (used to find all .clcpp files in a folder)
glob_helper = '@0@'.format(meson.current_source_dir()) + '/utility/glob_helper.py'

kernel_file_glob = run_command(prog_python, '@0@'.format(glob_helper), opencl_kernel_folder_path + '/*.clcpp', check : false).stdout().strip().split(',')

# Check if kernel_files_glob contains any files
if kernel_file_glob.length() == 0
  warning('''
      No kernel files found in folder: @0@
      '''.format(opencl_kernel_folder_path))
endif

clang = find_program('clang', required : true)
llvm_link = find_program('llvm-link', required : true)
opt = find_program('opt', required : true)
llc = find_program('llc', required : true)
llvm_spirv = find_program('llvm-spirv', required : true)

# Get all files in opencl_kernel_folder_path
kernel_files = files(kernel_file_glob)

opencl_dep = dependency('OpenCL')
opencl_headers = opencl_dep.get_pkgconfig_variable('includedir')

message('@0@'.format(opencl_headers))

kernel_data_folder = 'generated_llvm_and_spirv_files/'

list_of_kernels = []
# Print all files in opencl_kernel_folder_path
foreach file : kernel_files
  message('@0@'.format(file))
  kernel_name = fs.stem(file)
  # Usually you would expect to use nvptx64--nvidiacl as target,
  # but this causes the .ptr .global state space identifiers not be emitted
  # in the PTX assembly. This throws a compiler warning, as we link with
  # the clc library n-nptx64--nvidiacl target - but it works anyway ;)
  # See: https://github.com/llvm/llvm-project/issues/46954
  ptx_kernel_llvm = custom_target(kernel_name + '_ptx_llvm',
                         output : kernel_name + '.ptx.ll',
                         input : file,
                         command : [clang, '-x', 'cl','-c','-target', 'nvptx64--nvcl', '-emit-llvm', '-isystem','/usr/include/clc', '-include', '/usr/include/clc/clc.h','-cl-std=cl2.0','-S', '-o', '@OUTPUT@', '@INPUT@'],
                         build_by_default : true,
                         )
  spv_kernel_llvm = custom_target(kernel_name + '_spv_llvm',
                         output : kernel_name + '.spv.llvm',
                         input : file,
                         command : ['clang', '-cl-std=cl2.0', '-c', '-target', 'spir64', '-O0', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         build_by_default : true,
                         )
  message('@0@'.format(opencl_headers))

  ptx_kernel_llvm_linked = custom_target(kernel_name + '_ptx_linked',
                         output : kernel_name + '.ptx.bc',
                         input : ptx_kernel_llvm,
                         command : [llvm_link,'-S', '-o', '@OUTPUT@', '@INPUT@', '/usr/lib/clc/nvptx64--nvidiacl.bc'],
                         build_by_default : true,
                         )
  ptx_kernel_llvm_linked_optimized = custom_target(kernel_name + '_ptx_optimized',
                          output : kernel_name + '.opt.ptx.bc',
                          input : ptx_kernel_llvm_linked,
                          command : [opt,'-internalize', '-internalize-public-api-list=kernel', '-nvvm-reflect', '-O3', '@INPUT@', '-o', '@OUTPUT@'],
                          build_by_default : true,
                          )
  ptx_kernel = custom_target(kernel_name + '_ptx',
                         output : kernel_name + '.ptx.s',
                         input : ptx_kernel_llvm_linked_optimized,
                         command : [llc,'-O2','-mcpu=sm_60', '-march=nvptx64', '@INPUT@', '-o', '@OUTPUT@'],
                         build_by_default : true,
                         )

  spv_kernel = custom_target(kernel_name + '_spv',
                          output : kernel_name + '.spv.s',
                          input : spv_kernel_llvm,
                          command : [llvm_spirv, '@INPUT@', '-o', '@OUTPUT@'],
                          build_by_default : true)
  list_of_kernels += spv_kernel

  list_of_kernels += ptx_kernel
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
