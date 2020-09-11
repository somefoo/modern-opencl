#pragma once
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <vector>
#include "clw_context.hpp" 
#include "clw_helper.hpp"
#include <GL/gl.h>
namespace clw{
class foreign_memory {
 public:
  foreign_memory(const clw::context& context, GLuint texture, GLenum texture_target = GL_TEXTURE_2D, GLint mip_level = 0)
      : m_context(context) {
    cl_int error;

    m_foreign_array = clCreateFromGLTexture(context.get_cl_context(), CL_MEM_WRITE_ONLY, texture_target, mip_level, texture,&error);
    clw::opencl_throw_check(error);
  }

  ~foreign_memory() {
    if (m_foreign_array != NULL) {
        clw::opencl_throw_check(clReleaseMemObject(m_foreign_array));
    } else {
      //This should never happen.
      std::cerr << "Warning, double free of device mem. object.\n";
    }
  }
  // Delete special member functions for now,
  // can be implemented if needed
  foreign_memory(const foreign_memory&) = delete;
  foreign_memory(foreign_memory&&) = delete;
  foreign_memory& operator=(const foreign_memory&) = delete;
  foreign_memory& operator=(foreign_memory&&) = delete;

  /// Returns a reference to the internal opencl/gl object
  const cl_mem& get_device_reference() const{
    return m_foreign_array;
  }

  void release() const {
    clw::opencl_throw_check(clEnqueueReleaseGLObjects(m_context.get_cl_command_queue(), 1, &m_foreign_array, 0, NULL, NULL));
  }

  void acquire() const {
    clw::opencl_throw_check(clEnqueueAcquireGLObjects(m_context.get_cl_command_queue(), 1, &m_foreign_array, 0, NULL, NULL));
  }

 private:
  cl_mem m_foreign_array;
  const clw::context& m_context;
};
}
