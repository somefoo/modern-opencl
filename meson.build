project('ModernOpenCL', 'cpp', default_options : ['default_library=static', 'cpp_std=c++20'])


if meson.get_compiler('cpp').get_id() != 'clang'
	warning('ModernOpenCL requires Clang (to compile OpenCL kernels to SPIR-V).')
	message('You can use Clang by setting the CC and CXX environment variables to clang and clang++ respectively.')
	error('Wrong compiler')
endif


opencl_dep = dependency('OpenCL')
#modern_opencl_lib = static_library('modern-opencl')

compile_args = ['-fPIC', '-Og', '-Wall', '-Wextra']

sources = [
  'src/clw_context.cpp',
]

include_dirs = [
  'include/',
]

link_with = [
  opencl_dep,
]

modern_opencl_lib = static_library('modern-opencl', sources, include_directories : include_dirs, dependencies : link_with, c_args : compile_args, cpp_args : compile_args)

modern_opencl_dep = declare_dependency(version : '1.0', include_directories : include_dirs, dependencies : link_with, link_with : modern_opencl_lib)

prog_python = find_program('python3')
#spirv_to_header_conversion_tool = '@0@'.format(meson.current_source_dir()) + '/utility/spirv_to_header.py'
spirv_to_header_conversion_tool = '@0@'.format(meson.current_source_dir()) + '/utility/multiple_spirv_to_header.py'

# target_sources(['modern-opencl', 'PRIVATE', '${CMAKE_CURRENT_SOURCE_DIR}/src/clw_context.cpp'])
# set_target_properties(['modern-opencl', 'PROPERTIES', 'LINKER_LANGUAGE', 'CXX'])
# target_include_directories(['modern-opencl', 'PUBLIC', cmake_current_source_dir, '/include'])
# target_include_directories(['modern-opencl', 'PRIVATE', opencl_include_dirs])
# target_link_libraries(['modern-opencl', 'PRIVATE', opencl_libraries])
subdir('tests')
