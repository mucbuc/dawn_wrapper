#version 310 es
precision highp float;
precision highp int;

layout(binding = 0, r32f) uniform highp image2DArray arg_0;
void textureStore_dce0e2() {
  ivec2 v = ivec2(uvec2(1u));
  imageStore(arg_0, ivec3(v, int(1)), vec4(1.0f));
}
void main() {
  textureStore_dce0e2();
}
#version 310 es

layout(binding = 0, r32f) uniform highp image2DArray arg_0;
void textureStore_dce0e2() {
  ivec2 v = ivec2(uvec2(1u));
  imageStore(arg_0, ivec3(v, int(1)), vec4(1.0f));
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  textureStore_dce0e2();
}
