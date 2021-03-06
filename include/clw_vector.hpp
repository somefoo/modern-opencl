#pragma once
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <vector>
#include <cassert>
#include "clw_context.hpp"
#include "clw_helper.hpp"

namespace clw{
template <typename TDevice>
class vector {
  using TInternal = typename std::remove_const<TDevice>::type;
 public:
  /// Create a vector on context using already existing vector (r-value)
  vector(const clw::context& context, std::vector<TInternal>&& data, const bool push_on_construction = true)
      : m_context(&context) {
    cl_int error;
    m_host_array = data;

    if constexpr(std::is_const<TDevice>::value) {
        m_device_array =
            clCreateBuffer(context.get_cl_context(), CL_MEM_READ_ONLY,
                           m_host_array.size() * sizeof(TDevice), NULL, &error);
      }
    else {
      m_device_array =
          clCreateBuffer(context.get_cl_context(), CL_MEM_READ_WRITE,
                         m_host_array.size() * sizeof(TDevice), NULL, &error);
    }

    clw::opencl_throw_check(error, "Failed to create buffer.");
    if(push_on_construction){
      push();
    }
  }

  ~vector() {
    if (m_device_array != NULL) {
      clw::opencl_throw_check(clReleaseMemObject(m_device_array), "Failed to release memory object.");
      m_device_array = NULL;
    }
  }

  /// Create a vector on context with size specified by element_count
  vector(const clw::context& context, size_t element_count, const bool push_on_construction = true) : vector(context, std::vector<TInternal>(element_count), push_on_construction){
    //Do nothing but call other constructor
  }

  // Delete special member functions for now,
  // can be implemented if needed
  vector(const vector& other, const bool push_on_construction = true): vector(*other.m_context,std::vector<TInternal>(other.m_host_array), push_on_construction){};
  vector(vector&&) = delete;
  vector& operator=(const vector&) = delete;
  vector& operator=(vector&& other){
    assert(this != &other); //Moving object into itself... why?

    //Deallocate the device memory of current object
    if (m_device_array != NULL) {
      clw::opencl_throw_check(clReleaseMemObject(m_device_array), "Failed to release memory object on move.");
      m_device_array = NULL;
    }

    m_context      = std::move(other.m_context);
    m_device_array = std::move(other.m_device_array);
    m_host_array   = std::move(other.m_host_array);
    other.m_device_array = 0;
    other.m_context = 0;
    return *this;
  }

  //Allow access with [] operator
  TInternal& operator[](std::size_t index) {
    return m_host_array[index];
  }
  //Allow access with [] operator (for const)
  const TInternal& operator[](std::size_t index) const {
    return m_host_array[index];
  }

  /// Pushes host data to device
  void push() const{
    clw::opencl_throw_check(clEnqueueWriteBuffer(
        m_context->get_cl_command_queue(), m_device_array, CL_TRUE /*blocking*/,
        0, m_host_array.size() * sizeof(TDevice), m_host_array.data(), 0,
        NULL, NULL), "Failed to push data to device memory.");
  }
  /// Pulls device data
  void pull() {
        clw::opencl_throw_check(clEnqueueReadBuffer(
            m_context->get_cl_command_queue(), m_device_array,
            CL_TRUE /*blocking*/, 0, m_host_array.size() * sizeof(TDevice),
            m_host_array.data(), 0, NULL, NULL), "Failed to pull data into host memory.");
  }

  /// Returns a reference to the internal opencl object
  const cl_mem& get_device_reference() const{
    return m_device_array;
  }

  /// Returns the size of the vector
  size_t size() const{
    return m_host_array.size();
  }

  /// Return iterator to beginning of internal array
  auto begin(){
    return m_host_array.begin();
  }
  /// Return iterator to beginning of internal array
  auto begin() const{
    return m_host_array.begin();
  }
  /// Return iterator to end of internal array
  auto end(){
    return m_host_array.end();
  }
  /// Return iterator to end of internal array
  auto end() const{
    return m_host_array.end();
  }

 private:
  cl_mem m_device_array;
  std::vector<TInternal> m_host_array;
  //We are not the owner!
  const clw::context* m_context;
};
}
