#include "clw_context.hpp"
#include "clw_helper.hpp"
#include <stdexcept>
#include <vector>
#include <array>

///Helper function to return all platforms
const std::vector<cl_platform_id> get_platforms(){
  cl_uint platform_id_count = 0;
  clw::opencl_throw_check(clGetPlatformIDs(0, NULL, &platform_id_count), "Failed to obtain OpenCL platform IDs.");

  if(platform_id_count == 0){
    throw std::runtime_error("No OpenCL platform detected.");
  }

  std::vector<cl_platform_id> platform_ids(platform_id_count);
  clw::opencl_throw_check(clGetPlatformIDs(platform_id_count, platform_ids.data(), NULL), "Failed to obtain OpenCL platform IDs.");
  return platform_ids;
}

///Helper function to return all devices for a specific platform
const std::vector<cl_device_id> get_devices(const cl_platform_id platform){
  cl_uint device_id_count = 0;
  clw::opencl_throw_check(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &device_id_count), "Failed to obtain OpenCL device IDs.");

  if(device_id_count == 0){
    throw std::runtime_error("No OpenCL device detected.");
  }

  std::vector<cl_device_id> device_ids(device_id_count);
  clw::opencl_throw_check(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, device_id_count, device_ids.data(), NULL), "Failed to obtain OpenCL device IDs.");
  return device_ids;
}

clw::context::context(){
  cl_int error;
  auto platforms = get_platforms();
  auto devices = get_devices(platforms[0]);
  const auto device_to_use = 0;

  m_context = clCreateContext(0,1,&devices[device_to_use], NULL, NULL, &error);
  clw::opencl_throw_check(error, "Failed to create context.");
  m_command_queue = clCreateCommandQueue(m_context, devices[device_to_use], CL_QUEUE_PROFILING_ENABLE, &error);
  clw::opencl_throw_check(error, "Failed to create command queue.");
  m_device_id = devices[device_to_use];
}

clw::context::context(void * gl_context){
  cl_int error;
  auto platforms = get_platforms();
  auto devices = get_devices(platforms[0]);
  const auto device_to_use = 0;

  //For the properties:
  // https://www.khronos.org/registry/OpenCL/sdk/1.2/docs/man/xhtml/clCreateContext.html
  std::array<cl_context_properties, 5> properties
  {CL_GL_CONTEXT_KHR, reinterpret_cast<cl_context_properties>(gl_context),
   CL_GLX_DISPLAY_KHR, reinterpret_cast<cl_context_properties>(gl_context),
   0};

  m_context = clCreateContext(properties.data(),1,&devices[device_to_use], NULL, NULL, &error);
  clw::opencl_throw_check(error, "Failed to create context.");
  m_command_queue = clCreateCommandQueue(m_context, devices[device_to_use], CL_QUEUE_PROFILING_ENABLE, &error);
  clw::opencl_throw_check(error, "Failed to create command queue.");
  m_device_id = devices[device_to_use];
}

clw::context::~context(){
  clw::opencl_throw_check(clReleaseCommandQueue(m_command_queue), "Failed to release command queue.");
  clw::opencl_throw_check(clReleaseContext(m_context), "Failed to release context.");
}
const cl_context clw::context::get_cl_context() const{
  return m_context;
}

const cl_command_queue clw::context::get_cl_command_queue() const{
  return m_command_queue;
}

const cl_device_id clw::context::get_cl_device_id() const{
  return m_device_id;
}
