#pragma once
#include <CL/cl.h>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <vector>
#include <tuple>
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include "clw_context.hpp"
#include "clw_helper.hpp"
#include "clw_vector.hpp"
#include "clw_stubs.hpp"

#ifndef KERNEL_DIR
#define KERNEL_DIR ""
#endif
namespace clw{
class function{
  public:
  const std::string do_include(const std::string& path, std::vector<std::string>& already_included){
    std::filesystem::path path_to_file(path);
    std::filesystem::path directory_of_file = path_to_file.parent_path();
    std::ifstream file_stream(path_to_file);

    std::string code_string = "";
  
    //Ensure each file is only included once!
    if(std::find(already_included.begin(), already_included.end(), path_to_file.lexically_normal()) != already_included.end()){
      file_stream.close();
      return "";
    }

    if(file_stream.fail()){
      std::string error_message = "Failed to open file: ";
      error_message += path_to_file;
      throw std::invalid_argument(error_message);
    }

    already_included.push_back(path_to_file.lexically_normal());
    
    
    std::string line;
    while(std::getline(file_stream, line)){
      size_t include_position = line.find("#clw_include_once");
      if(include_position != std::string::npos){
        size_t include_start = line.find_first_of('"', include_position);
        size_t include_end = line.find_first_of('"', include_start + 1);
        if(include_start == std::string::npos || include_end == std::string::npos){
          throw std::logic_error("Error when parsing OpenCL program: #cl_include not of the expected format: clw_include_once \"...\"");
        }
        include_start++;
        size_t include_size = include_end - include_start;
        size_t include_size_all = include_end - include_position + 1;
        
        std::string include_file_name = line.substr(include_start, include_size);

        std::filesystem::path tmp_dir = directory_of_file;
        std::string include_content = do_include(directory_of_file/include_file_name, already_included);
        line.replace(include_position, include_size_all, include_content);
        code_string += line + '\n';
      }else{
        code_string += line + '\n'; 
      }
    }
    file_stream.close();

    return code_string;
  }


  function(const clw::context& context, const std::string& name, const std::string& function_name): m_function_name(function_name), m_name(name), m_context(&context){
    //Generate CL code 
    cl_int error;

    // TODO
    // It seems clang doesn't emit the .ptr .global modifiers for the pointers in the NVPTX,
    // if these modifiers are added manually, the code runs.

    if(context.supports_extension("cl_khr_il_program")){
      std::vector<uint8_t> binary_program = clw_generated::get_program_bytecode(name + "_spv");
      m_program = clCreateProgramWithIL(m_context->get_cl_context(), binary_program.data(), binary_program.size(), &error);
    }
    else{
      // Nvidia does not implement clCreateProgramWithIL in OpenCL 3.0
      std::vector<uint8_t> binary_program = clw_generated::get_program_bytecode(name + "_ptx");

      std::size_t binary_length = binary_program.size(); 
      auto cl_device_id = m_context->get_cl_device_id();
      auto binary = static_cast<const unsigned char*>(binary_program.data());

      cl_int binary_status = 0;
      m_program = clCreateProgramWithBinary(m_context->get_cl_context(), 1, &cl_device_id, &binary_length, &binary, &binary_status, &error);

      std::cout << "Binary status: " << binary_status << '\n';
    }

  


    clw::opencl_throw_check(error, "Failed to create program from intermediate language.");

    //error = clBuildProgram(m_program, 0, NULL, "-cl-mad-enable -cl-std=CL1.2", NULL, NULL);
    error = clBuildProgram(m_program, 0, NULL, "", NULL, NULL);
    if(error != CL_SUCCESS){
      //Failed, print the error log
      std::cout << "Kernel failed to compile:\n";
      std::array<char, 4096> buffer;
      size_t len;
      cl_int error = clGetProgramBuildInfo(m_program, m_context->get_cl_device_id(), CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer.data(), &len);
      clw::opencl_throw_check(error, "Failed to retrieve error message.");

      std::cout << "---------------------------------------\n";
      std::cout << "\033[1;33m" << buffer.data() << "\033[0m" << '\n';
      std::cout << "---------------------------------------\n";
    }
    clw::opencl_throw_check(error, "Failed to build program.");

    cl_build_status build_status;
    error = clGetProgramBuildInfo(m_program, m_context->get_cl_device_id(), CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);



    m_kernel = clCreateKernel(m_program, function_name.data(), NULL);
    clw::opencl_throw_check(error, "Failed to create kernel.");
  }

  ~function(){
    if (m_kernel != NULL) {
      clw::opencl_throw_check(clReleaseKernel(m_kernel), "Failed to release kernel.");
    }
    if(m_program != NULL){
      clw::opencl_throw_check(clReleaseProgram(m_program), "Failed to release program.");
    }
  }

  function(const function&) = delete;
  function(function&&) = delete;
  //Returning new object to prevent device side memory problems
  [[deprecated("Warning, consider using the move assignment instead; it is much faster.")]] function operator=(const function& other){
    return function(*other.m_context,other.m_name,other.m_function_name); 
  }
  function& operator=(function&& other){
    assert(this != &other); //Moving object into itself... why?

    if (m_kernel != NULL && m_program != NULL) {
        clw::opencl_throw_check(clReleaseKernel(m_kernel), "Failed to release kernel on move.");
        clw::opencl_throw_check(clReleaseProgram(m_program), "Failed to release program on move.");
    }

    m_context        = std::move(other.m_context);
    m_kernel         = std::move(other.m_kernel);
    m_program        = std::move(other.m_program);
    other.m_context  = NULL;
    other.m_kernel   = NULL;
    other.m_program  = NULL;

    m_name           = std::move(other.m_name);
    m_function_name  = std::move(other.m_function_name);
    return *this;
  };



  template <size_t Pos, typename Arg, typename ...Rest>
  void recurse_helper(Arg& a, Rest&... rest) const{
    cl_int error;


    if constexpr (std::is_arithmetic<typename std::remove_reference<Arg>::type>::value){
      error = clSetKernelArg(m_kernel, Pos, sizeof(a), &a);
    }else{
      error = clSetKernelArg(m_kernel, Pos, sizeof(a.get_device_reference()), &(a.get_device_reference()));
    }

    if constexpr (sizeof...(rest) > 0){
      recurse_helper<Pos + 1>(rest...);
    }

    clw::opencl_throw_check(error, "Failed to set argument for kernel.");
  }

  /// @tparam GX the global X size
  /// @tparam GY the global Y size
  /// @tparam GZ the global Z size
  /// @tparam LX the local Y size
  /// @tparam LY the local Y size
  /// @tparam LZ the local Z size
  template <size_t GX, size_t GY, size_t GZ, size_t LX, size_t LY, size_t LZ, typename ...Args>
  [[deprecated("Use 'execute(...) instead'")]] void run(const Args&... arg) const{
    static_assert(GX >= LX, "Error, global size x < local_size x.");
    static_assert(GY >= LY, "Error, global size y < local size y.");
    static_assert(GZ >= LZ, "Error, global size z < local size z.");
    static_assert((GX % LX) == 0, "Error, global size x is not a multiple of local size x.");
    static_assert((GY % LY) == 0, "Error, global size y is not a multiple of local size y.");
    static_assert((GZ % LZ) == 0, "Error, global size z is not a multiple of local size z.");
    recurse_helper<0>(arg...);
    cl_int error;

    constexpr const std::array<size_t, 3> global_size{GX, GY, GZ};
    constexpr const std::array<size_t, 3> local_size{LX, LY, LZ};
    error = clEnqueueNDRangeKernel(m_context->get_cl_command_queue(), m_kernel, global_size.size(), NULL, global_size.data(), local_size.data(), 0, NULL, NULL);

    //clFinish(m_context.get_cl_command_queue());
    clw::opencl_throw_check(error, "Failed to execute kernel.");
  }

  /// @tparam GX the global X size
  /// @tparam GY the global Y size
  /// @tparam LX the local Y size
  /// @tparam LY the local Y size
  template <size_t GX, size_t GY, size_t LX, size_t LY, typename ...Args>
  [[deprecated("Use 'execute(...) instead'")]] void run(const Args&... arg) const{
    run<GX,GY,1,LX,LY,1>(arg...);
  }


  /// @tparam GX the global X size
  /// @tparam LX the local Y size
  template <size_t GX, size_t LX, typename ...Args>
  [[deprecated("Use 'execute(...) instead'")]] void run(const Args&... arg) const{
    run<GX,1,1,LX,1,1>(arg...);
  }


  /// @param global_size a vector representing the global size, it is suggested
  //         to use initilizer_lists, example: 1D: {256}, 2D: {256,128} or 3D: {256,128,512}
  /// @param local_size a vector representing the local size
  //         to use initilizer_lists, example: 1D: {64}, 2D: {8,8} or 3D: {4,4,4}
  template<typename ...Args>
  constexpr void execute(std::array<size_t, 3> global_size, std::array<size_t, 3> local_size, const Args&... arg) const{
    //Ensure that OpenCL does not cause issues with work_sizes of 0
    constexpr auto remove_zero = [](auto& array){
      for (auto& val : array){
        if(val == 0) val = 1;
      }
    };
    remove_zero(global_size);
    remove_zero(local_size);

    if(local_size[0]*local_size[1]*local_size[2] > 256){
      std::cerr << "Warning, creating local size incompatible with AMD GPUs.\n";
      std::cerr << "  used local size: " << local_size[0]*local_size[1]*local_size[2] << " > 256\n";
    }

    if(!(global_size[0] >= local_size[0])) throw std::invalid_argument("Global size is smaller than local size: X."); //Error, global size x < local_size x.
    if(!(global_size[1] >= local_size[1])) throw std::invalid_argument("Global size is smaller than local size: Y."); //Error, global size y < local_size y.
    if(!(global_size[2] >= local_size[2])) throw std::invalid_argument("Global size is smaller than local size: Z.");; //Error, global size z < local_size z.
    if(!((global_size[0] % local_size[0]) == 0)) throw std::invalid_argument("Global size is not a multiple of local size: X."); //Error, global size x is not a multiple of local size x
    if(!((global_size[1] % local_size[1]) == 0)) throw std::invalid_argument("Global size is not a multiple of local size: Y."); //Error, global size y is not a multiple of local size y
    if(!((global_size[2] % local_size[2]) == 0)) throw std::invalid_argument("Global size is not a multiple of local size: Z."); //Error, global size z is not a multiple of local size z

    recurse_helper<0>(arg...);
    cl_int error{0};
    error = clEnqueueNDRangeKernel(m_context->get_cl_command_queue(), m_kernel, global_size.size(), NULL, global_size.data(), local_size.data(), 0, NULL, NULL);
    //clFinish(m_context.get_cl_command_queue());
    clw::opencl_throw_check(error, "Failed to execute kernel.");
  }

private:
  cl_program m_program;
  cl_kernel m_kernel;

  std::string m_function_name;
  std::string m_name;
  const clw::context* m_context;
};
}
