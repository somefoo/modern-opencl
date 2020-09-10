#include <clw_context.hpp>
#include <clw_vector.hpp>
#include <clw_function.hpp>
#include <vector>

bool run_vector_test_0(){
  clw::context test_context;

  constexpr const int size = 1 << 14;


  clw::vector<int> data(test_context, std::vector<int>(size), false);
  for(int i = 0; i < size; ++i){
   data[i] = i;
  }
  data.push();

  clw::function f(test_context, "quick_test.cl", "run");
  f.execute({size,0,0}, {32,0,0}, data, size, 42);
  data.pull();

  for(int i = 0; i < size; ++i){
   if(data[i] != i + 42) return true;
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

  clw::function f(test_context, "quick_test.cl", "run");
  f.execute({size,0,0}, {32,0,0}, data, size, 42);
  data.pull();

  for(int i = 0; i < size; ++i){
   if(data[i] != i + 42) return true;
  }
  return false;
}

int main(){
  bool failed = false;
  failed |= run_vector_test_0();
  failed |= run_vector_test_1();
  return failed;
}
