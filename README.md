# modern-opencl
A modern abstraction layer for creating OpenCL applications.

# WIP ;)
The code is still WIP; major changes may happen to the structure and function signatures.

# Example
Here I will give examples for how to use this library. Two CPP code sections are shown, see the long version for descriptions and examples of some extra functionality.

## C++ Code (Short)
```C++
////CPP FILE

#include <clw_context.hpp>
#include <clw_vector.hpp>
#include <clw_function.hpp>
#include <vector>
int main(){
  clw::context     context;
  clw::vector<int> data(context, std::vector<int>{1,2,3,4});
  clw::function    function(context, "path/to/quick_test.cl", "run");
  function.execute({4,0,0}, {4,0,0}, data, 4, 42);
  data.pull();
  return data[0] + data[1] + data[2];
}
```
## C++ Code (Long)
```C++
////CPP FILE

#include <clw_context.hpp>
#include <clw_vector.hpp>
#include <clw_function.hpp>
#include <vector>
int main(){
  clw::context test_context; //Create context (device 0 is always selected for now)

  //Create array of int with 1 << 14 elements, and don't push to device (i.e GPU) memory
  constexpr const int size = 1 << 14; 
  clw::vector<int> data(test_context, size, false /*do not push to device on init.*/);
  
  //Give each value in data a unqiue value
  for(int i = 0; i < size; ++i){
   data[i] = i;
  }
  
  //Push data to device (i.e GPU) memory
  data.push();

  //Create a function (kernel) object from the function called run in file quick_test.cl
  clw::function f(test_context, "path/to/quick_test.cl", "run", "#define SOME_RANDOM_STUFF");
  
  //Run the function (kernel) f on the device (i.e GPU) with:
  // Global size (size, 0, 0) = (size, 1, 1)
  // Local size  (32, 0, 0)   = (32, 1, 1)
  // First parameter  = data (the array defined previously)
  // Second parameter = size
  // Third parameter  = 42
  f.execute({size,0,0}, {32,0,0}, data, size, 42);
  
  //Pull data into host memory
  data.pull();

  //Check if kernel executed correctly
  for(int i = 0; i < size; ++i){
   if(data[i] != i + 42) return 1;
  }

  return 0;
}
```
## Kernel Code
```C
////CL FILE

//Include some_file.cl (with multi-include guards)
#clw_include_once "some_file.cl"

__kernel void run(__global int* values, int count, int value_to_add){
  int3 id = {get_global_id(0), get_global_id(1), get_global_id(2)};
  if(id.x >= count){
    return;
  }

  values[id.x] += value_to_add;
}
```

# Notes
This wrapper started out as part of a project which can be found [**here**](https://github.com/cl-volume-renderer/cl-volume-renderer).
