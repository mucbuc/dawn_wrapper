#version 460
precision highp float;
precision highp int;

layout(binding = 0, rg32i) uniform highp iimage2DArray arg_0;
void textureStore_ea30d2() {
  uvec2 arg_1 = uvec2(1u);
  int arg_2 = 1;
  ivec4 arg_3 = ivec4(1);
  int v = arg_2;
  ivec4 v_1 = arg_3;
  ivec2 v_2 = ivec2(arg_1);
  imageStore(arg_0, ivec3(v_2, int(v)), v_1);
}
void main() {
  textureStore_ea30d2();
}
#version 460

layout(binding = 0, rg32i) uniform highp iimage2DArray arg_0;
void textureStore_ea30d2() {
  uvec2 arg_1 = uvec2(1u);
  int arg_2 = 1;
  ivec4 arg_3 = ivec4(1);
  int v = arg_2;
  ivec4 v_1 = arg_3;
  ivec2 v_2 = ivec2(arg_1);
  imageStore(arg_0, ivec3(v_2, int(v)), v_1);
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  textureStore_ea30d2();
}
