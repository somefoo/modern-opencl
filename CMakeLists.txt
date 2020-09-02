cmake_minimum_required (VERSION 2.8.11)
project(ModernOpenCL)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-fPIC -Og -Wall -Wextra")

find_package(OpenCL REQUIRED)


add_library(modern-opencl STATIC)
target_sources(modern-opencl
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/clw_context.cpp"
#  PUBLIC
#    "${CMAKE_CURRENT_SOURCE_DIR}/include/clw_context.hpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/include/clw_image.hpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/include/clw_vector.hpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/include/clw_function.hpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/include/clw_helper.hpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/include/clw_foreign_memory.hpp"
  )
set_target_properties(modern-opencl PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(modern-opencl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(modern-opencl PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(modern-opencl PRIVATE ${OpenCL_LIBRARIES})

ENABLE_TESTING()
ADD_SUBDIRECTORY(tests)
