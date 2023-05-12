#include <clw_context.hpp>
#include <clw_vector.hpp>
//#include <clw_function.hpp>
#include <clw_precompiled_function.hpp>
#include <clw_stubs.hpp>
#include <vector>
#include <CL/cl.h>
#include <iostream>

bool run_vector_test_0(){
  clw::context test_context;

  auto context = test_context.get_cl_context();
  cl_int err;
  //void (*test_kernel)(cl_mem, cl_mem, int) = (void (*)(cl_mem, cl_mem, int))clGetKernelFunctionPointer(context, "run", &err);

  constexpr const int size = 1 << 14;


  clw::vector<int> data(test_context, std::vector<int>(size), false);
  for(int i = 0; i < size; ++i){
   data[i] = i;
  }
  data.push();

  clw::function f(test_context, "quick_test", "run");
  f.execute({size,0,0}, {32,0,0}, data, size, 42);
  data.pull();

  int i = 0;
  for(auto d : data){
   if(d != i + 42) return true;
   i++;
  }
  return false;
}

bool run_vector_test_1(){
  clw::context test_context;

  constexpr const int size = 1 << 14;


  clw::vector<int> data(test_context, size, false);
  for(int i = 0; i < size; ++i){
   data[i] = i;
  }
  data.push();

  clw::function f(test_context, "quick_test", "run");
  f.execute({size,0,0}, {32,0,0}, data, size, 42);
  data.pull();

  for(int i = 0; i < size; ++i){
   if(data[i] != i + 42) return true;
  }
  return false;
}

bool run_vector_test_2(){
  clw::context test_context;

  clw_generated::get_program_bytecode("quick_test");

  // Ensure the same call with the string b___this_function_does_not_exist___a will fail by throwing an exception
  try{
    clw_generated::get_program_bytecode("b___this_function_does_not_exist___a");
    return true;
  }catch(...){
    // Expected
  }

  return false;
}





int main(){
  bool failed = false;
  failed |= run_vector_test_0();
  failed |= run_vector_test_1();
  failed |= run_vector_test_2();

  if (failed) {
    std::cout << "Failed\n";
  }
  else{
    std::cout << "Passed\n";
  }
  return failed;
}
