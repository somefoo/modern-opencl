#pragma once
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <array>
#include <cassert>
#include "clw_context.hpp"
#include "clw_helper.hpp"
namespace clw{
template <typename TDevice, size_t ChannelSize=1>
class image {
  using TInternal = typename std::remove_const<TDevice>::type;
  using TInternalConst = typename std::add_const<TInternal>::type;
  friend class image<TInternalConst, ChannelSize>;
  friend class image<TInternal, ChannelSize>;
 public:
  constexpr image(const clw::context& context, std::vector<TInternal>&& data, std::array<size_t, 3> dimensions, const bool push_on_construction = false)
      : m_context(&context) {
    cl_int error{0};
    m_host_array = data;

		//Ensure that OpenCL does not cause issues with work_sizes of 0
		constexpr auto remove_zero = [](auto& array){
			for (auto& val : array){
				if(val == 0) val = 1;
			} 
		};
		remove_zero(dimensions);
		m_dimensions = dimensions;
		

    const auto width = dimensions[0];
    const auto height = dimensions[1];
    const auto depth = dimensions[2];

    if(width * height * depth * ChannelSize != m_host_array.size()){
      std::cerr << "Error, moved array is not of the correct size\n";
      std::cerr << " Expected: " << width*height*depth*ChannelSize << '\n';
      std::cerr << " Received: " << m_host_array.size() << '\n';
      throw std::invalid_argument("Array size is not the same as image size.");
    }

    const auto eval_image_type = [&](){
      if(!(width > 1 && height >= 1 && depth >= 1)){
        throw std::invalid_argument("Image size is not valid.");
      }
      if(width > 1 && height == 1 && depth == 1){
        return CL_MEM_OBJECT_IMAGE1D;
      }else if    (width > 1 && height > 1 && depth == 1){
        return CL_MEM_OBJECT_IMAGE2D; 
      }else if    (width > 1 && height > 1 && depth > 1){
        return CL_MEM_OBJECT_IMAGE3D; 
      }else{
        std::cerr << "Error, image size is not valid \n";
        throw std::invalid_argument("Image size is not valid.");
      }
    };
    
    constexpr const auto eval_access_right = [&](){
      if constexpr(std::is_const<TDevice>::value) {
        return CL_MEM_READ_ONLY;
      }else{
        return CL_MEM_READ_WRITE;
      }
    };

    constexpr const auto eval_image_format = [&](){
      cl_image_format format{0};
      
      if constexpr(ChannelSize == 1){
        format.image_channel_order = CL_R; 
      }else if(ChannelSize == 2){
        format.image_channel_order = CL_RG; 
      }else if(ChannelSize == 4){
        format.image_channel_order = CL_RGBA; 
      }else{
        //We have no constexpr fail :(
        static_assert(ChannelSize < 5 && ChannelSize != 3 && ChannelSize > 0, "Invalid channel size.");
      }

      constexpr bool is_signed = std::is_signed<TDevice>::value;
      constexpr size_t type_size = sizeof(TDevice);
      constexpr bool is_integral = std::is_integral<TDevice>::value;
      static_assert(type_size <= 4, "Error, only 32 bit or smaller values are supported.");
      if constexpr(is_integral){
        if constexpr(is_signed){
          if constexpr(type_size == 1){
            format.image_channel_data_type = CL_SIGNED_INT8;
          }else if(type_size == 2){
            format.image_channel_data_type = CL_SIGNED_INT16;
          }else{
            format.image_channel_data_type = CL_SIGNED_INT32;
          }
        }else{
          if constexpr(type_size == 1){
            format.image_channel_data_type = CL_UNSIGNED_INT8;
          }else if(type_size == 2){
            format.image_channel_data_type = CL_UNSIGNED_INT16; 
          }else{
            format.image_channel_data_type = CL_UNSIGNED_INT32;
          }
        }
      }else{
        static_assert(type_size == 4, "Error, only 32 bit floats are supported.");
        format.image_channel_data_type = CL_FLOAT;
      }
      return format;
    };

    const auto eval_image_desc = [&](const auto image_type){
      cl_image_desc desc{0};
      desc.image_type = image_type;
      desc.image_width = width;
      desc.image_height = height;
      desc.image_depth = depth;
      desc.image_array_size = 1;
      desc.image_row_pitch = 0; //TODO correct?
      desc.image_slice_pitch = 0; //TODO correct?
      desc.num_mip_levels = 0;
      desc.num_samples = 0;
      desc.buffer = NULL; //TODO correct?
      return desc;
    };

    const auto image_type =              eval_image_type();
    const auto access_right =            eval_access_right();
    const cl_image_format image_format = eval_image_format();
    const cl_image_desc   image_desc =   eval_image_desc(image_type);

    m_device_array = clCreateImage(context.get_cl_context(), access_right, &image_format, &image_desc, NULL, &error);
    clw::opencl_throw_check(error, "Failed to create image.");
    if(push_on_construction){
      push();
    }
  }

  ~image() {
    if (m_device_array != NULL) {
      clw::opencl_throw_check(clReleaseMemObject(m_device_array), "Failed to release image.");
      m_device_array = NULL;
    }
  }
  // Delete special member functions for now,
  // can be implemented if needed
  image(const image& other, const bool push_on_construction = false): image(*other.m_context, std::vector<TInternal>(other.m_host_array),other.m_dimensions,push_on_construction){
  };
  image(image&&) = delete;
  image& operator=(image&) = delete;
  image& operator=(image&& other){
    assert(this != &other); //Moving object into itself... why?
    
    //Deallocate the device memory of the current object
    if (m_device_array != NULL) {
      clw::opencl_throw_check(clReleaseMemObject(m_device_array), "Failed to release image on move.");
      m_device_array = NULL;
    }


    m_context      = std::move(other.m_context);
    m_dimensions   = std::move(other.m_dimensions);
    m_device_array = std::move(other.m_device_array);
    m_host_array   = std::move(other.m_host_array);
    other.m_device_array = 0;
    other.m_context = 0;
    return *this;
  }

  //Allow assignment to other image via-copy. This also allows
  //assignments like: image<const T> = image<T>;
  //This function does not push!
  void host_copy_from(const image<TInternalConst, ChannelSize>& other){
    if(this->m_dimensions != other.m_dimensions){
      throw std::invalid_argument("Dimensions are not compatible.");
    }
    this->m_device_array = other.m_device_array;
  }

  //Allow assignment to other image via-copy. This also allows
  //assignments like: image<T> = image<const T>;
  //This function does not push!
  void host_copy_from(const image<TInternal, ChannelSize>& other){
    if(this->m_dimensions != other.m_dimensions){
      throw std::invalid_argument("Dimensions are not compatible.");
    }
    this->m_device_array = other.m_device_array;
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
    std::array<size_t, 3> origin{0,0,0};
    std::array<size_t, 3> region{m_dimensions[0],m_dimensions[1],m_dimensions[2]};
    clw::opencl_throw_check(clEnqueueWriteImage(
        m_context->get_cl_command_queue(), m_device_array, CL_TRUE /*blocking*/,
        origin.data(),region.data(),0,0,m_host_array.data(),0,NULL,NULL));
  }
  /// Pulls device data
  void pull() {
    std::array<size_t, 3> origin{0,0,0};
    std::array<size_t, 3> region{m_dimensions[0],m_dimensions[1],m_dimensions[2]};
    clw::opencl_throw_check(clEnqueueReadImage(
        m_context->get_cl_command_queue(), m_device_array, CL_TRUE /*blocking*/,
        origin.data(),region.data(),0,0,m_host_array.data(),0,NULL,NULL));
  }

  /// Returns a reference to the internal opencl object
  const cl_mem& get_device_reference() const{
    return m_device_array;
  }

  /// Returns the size of the vector
  size_t size() const{
    return m_host_array.size();
  }

  /// Returns the number of pixels
  /// Each pixel is a combination of values
  /// depending on the number of channels
  size_t pixel_count() const{
    return m_host_array.size()/ChannelSize;
  }

  /// Returns the number of channels per pixel
  size_t channels() const{
    return ChannelSize;
  }

  /// Returns the dimensions of the image
  const std::array<size_t,3>& get_dimensions(){
    return m_dimensions;
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
  const clw::context* m_context;
  std::array<size_t, 3> m_dimensions;
};
}
