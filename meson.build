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
kernel_files = files(opencl_kernel_folder_path + '/quick_precompiled_test.clcpp')
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
                         output : kernel_name + '.ll',
                         input : file,
                         #command : ['clang', '-cl-std=CLC++2021', '-c', '-target', 'spirv64', '-O0', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang', '-cl-std=CLC++2021', '-c', '-target', 'spir64-unknown-unknown', '-O0', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang', '-cl-std=CLC++2021', '-c', '-target', 'spir64-unknown-unknown', '-O0', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang', '-cl-std=CLC++2021', '-c', '-target', 'spirv64', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang-15', '-cl-std=CLC++2021', '-c', '-target', 'spirv64-unknown-unknown', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang-15', '-cl-std=CLC++2021', '-c', '-target', 'spirv64', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang-15', '-target', 'spirv64-unknown-unknown', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang-15', '-target', 'spirv64', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang-15', '-cl-std=CLC++2021', '-c', '-target', 'spirv64-unkown-unkown', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang-15', '-cc1', '-emit-spirv', '-triple', 'spir64-unknown-unknown', '-x', 'cl', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],


                         #command : ['clang-15', '-cl-std=CLC++2021', '-c', '-target', 'spirv64-unkown-unkown', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang-15', '-Dcl_clang_storage_class_specifiers', '-isystem', 'libclc/generic/include','-cl-std=CLC++2021', '-c', '-target', 'nvptx', '-emit-llvm', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang', '-x', 'cl', '-target', 'nvptx64', '-cl-std=CLC++2021', '-S' ,'-emit-llvm','-Xclang','-finclude-default-header', '-Xclang', '-fdeclare-opencl-builtins', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang', '-x', 'cl', '-target', 'nvptx64', '-cl-std=CLC++2021', '-S' ,'-emit-llvm','-Xclang','-finclude-default-header', '-Xclang', '-fdeclare-opencl-builtins', '-I', opencl_headers, '@INPUT@', '-o', '@OUTPUT@'],


                         #  -Dcl_clang_storage_class_specifiers -include /usr/include/clc/clc.h -target nvptx64--nvidiacl
                         #command : ['clang', '-Dcl_clang_storage_class_specifiers', '-isystem','/usr/include/clc', '-include', 'clc.h', '-x', 'cl', '-target', 'nvptx64--nvidiacl', '-cl-std=CLC++2021', '-S' ,'-emit-llvm','@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang', '-Dcl_clang_storage_class_specifiers', '-isystem','/usr/include/clc', '-include', 'clc.h', '-Xclang' ,'-finclude-default-header', '-Xclang', '-fdeclare-opencl-builtins', '-x', 'cl', '-target', 'nvptx64--nvidiacl', '-cl-std=CLC++2021', '-S' ,'-emit-llvm','@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang-15', '-Dcl_clang_storage_class_specifiers', '-isystem','/usr/include/clc', '-include', 'clc.h', '-Xclang' ,'-finclude-default-header', '-Xclang', '-fdeclare-opencl-builtins', '-x', 'cl', '-target', 'nvptx64--nvidiacl', '-cl-std=CL3.0','-Xclang','-cl-ext=+__opencl_c_images', '-S' ,'-emit-llvm','@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang-15', '-Dcl_clang_storage_class_specifiers', '-isystem','/usr/include/clc', '-include', 'clc.h', '-Xclang' ,'-finclude-default-header', '-Xclang', '-fdeclare-opencl-builtins', '-x', 'cl', '-target', 'nvptx64--nvidiacl', '-cl-std=CLC++2021','-Xclang','-cl-ext=+__opencl_c_images', '-S' ,'-emit-llvm','@INPUT@', '-o', '@OUTPUT@'],
                         #command : ['clang', '-Dcl_clang_storage_class_specifiers', '-isystem','/usr/include/clc', '-include', 'clc.h', '-cl-std=CLC++2021', '-Xclang', '-cl-ext=+__opencl_c_images', '-xcl', '-target', 'nvptx64--nvidiacl', '-emit-llvm','@INPUT@', '-S', '-o', '@OUTPUT@'],
                         #command : ['clang-15', '-Xclang', '-no-opaque-pointers','-Dcl_clang_storage_class_specifiers', '-isystem','/usr/include/clc', '-include', 'clc.h', '-Xclang','-cl-std=CLC++2021','-cl-ext=+__opencl_c_images', '-xcl', '-target', 'nvptx64--nvidiacl', '-emit-llvm','@INPUT@', '-S', '-o', '@OUTPUT@'],
                         #command : ['clang', '-Xclang', '-no-opaque-pointers','-Dcl_clang_storage_class_specifiers', '-isystem','/usr/include/clc', '-include', 'clc.h', '-Xclang','-cl-std=CL3.0','-cl-ext=+__opencl_c_images', '-xcl', '-target', 'nvptx64--nvidiacl', '-emit-llvm','@INPUT@', '-S', '-o', '@OUTPUT@'],
                         #command : ['clang', '-Xclang', '-Dcl_clang_storage_class_specifiers', '-isystem','/usr/include/clc', '-include', 'clc.h', '-Xclang','-cl-std=CL3.0','-Xclang','-cl-ext=+__opencl_c_images', '-xcl', '-target', 'nvptx64--nvidiacl', '-emit-llvm','@INPUT@', '-S', '-o', '@OUTPUT@'],
                         command : ['clang','-Dcl_clang_storage_class_specifiers','-isystem','/usr/include/clc', '-include', '/usr/include/clc/clc.h', '-target', 'nvptx64--nvidiacl','-xcl', '@INPUT@', '-emit-llvm', '-S', '-o', '@OUTPUT@'],

                         # Works:
                         #command : ['clang', '-Dcl_clang_storage_class_specifiers', '-isystem','/usr/include/clc', '-include', 'clc.h', '-xcl', '-target', 'nvptx64--nvidiacl', '-emit-llvm','@INPUT@', '-S', '-o', '@OUTPUT@'],
                         #command : ['clang','-Dcl_clang_storage_class_specifiers','-isystem','/usr/include/clc', '-include', '/usr/include/clc/clc.h', '-target', 'nvptx64--nvidiacl','-xcl','@INPUT@', '-emit-llvm', '-S', '-o', '@OUTPUT@'],
                         

                         build_by_default : true,
                         )
  message('@0@'.format(opencl_headers))

  kernel_llvm_linked = custom_target(kernel_name + '_linked',
                         output : kernel_name + '.bc',
                         input : kernel_llvm,
                         #command : ['llvm-link-15', '@INPUT@','/usr/lib/clc/nvptx64--nvidiacl.bc','-S', '-o', '@OUTPUT@'],
                         #command : ['llvm-link', '@INPUT@', '/usr/lib/clc/nvptx64--nvidiacl.bc','-S', '-o', '@OUTPUT@'],
                         #command : ['llvm-link', '/usr/lib/clc/nvptx64--nvidiacl.bc', '@INPUT@','-S', '-o', '@OUTPUT@'],
                         #command : ['llvm-link', '@INPUT@', '/usr/lib/clc/nvptx64--nvidiacl.bc', '-S', '-o', '@OUTPUT@'],
                         #command : ['llvm-link', '@INPUT@', '/usr/lib/clc/nvptx64--nvidiacl.bc', '-v', '-S', '-o', '@OUTPUT@'],
                         command : ['llvm-link', '@INPUT@', '/usr/lib/clc/nvptx64--nvidiacl.bc', '-v', '-S', '-o', '@OUTPUT@'],
                         build_by_default : true,
                         )


  kernel_spv = custom_target(kernel_name + '_ptx',
                         output : kernel_name + '.s',
                         input : kernel_llvm_linked,
                         command : ['clang', '-target', 'nvptx64--nvidiacl', '@INPUT@', '-S','-o', '@OUTPUT@'],
                         build_by_default : true,
                         )



  #kernel_spv = custom_target(kernel_name + '_ptx',
  #                       output : kernel_name + '.ptx',
  #                       input : kernel_llvm_linked,
  #                       command : ['llc','-mcpu=sm_60', '-march=nvptx64', '@INPUT@', '-o', '@OUTPUT@'],
  #                       build_by_default : true,
  #                       )

  #kernel_spv = custom_target(kernel_name,
  #                        output : kernel_name + '.spv',
  #                        input : kernel_llvm,
  #                        command : ['llvm-spirv-15', '@INPUT@', '-o', '@OUTPUT@'],
  #                        build_by_default : true)
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
