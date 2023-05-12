__kernel void run(__global int* values, int count, int value_to_add){
  int3 id = {get_global_id(0), get_global_id(1), get_global_id(2)};
  if(id.x >= count){
    return;
  }

  values[id.x] += value_to_add;
}
