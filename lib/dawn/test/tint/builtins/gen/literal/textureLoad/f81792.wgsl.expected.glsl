#version 310 es
precision highp float;
precision highp int;

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  vec4 inner;
} v;
layout(binding = 0, r32f) uniform highp image2DArray arg_0;
vec4 textureLoad_f81792() {
  ivec2 v_1 = ivec2(ivec2(1));
  vec4 res = imageLoad(arg_0, ivec3(v_1, int(1)));
  return res;
}
void main() {
  v.inner = textureLoad_f81792();
}
#version 310 es

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  vec4 inner;
} v;
layout(binding = 0, r32f) uniform highp image2DArray arg_0;
vec4 textureLoad_f81792() {
  ivec2 v_1 = ivec2(ivec2(1));
  vec4 res = imageLoad(arg_0, ivec3(v_1, int(1)));
  return res;
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  v.inner = textureLoad_f81792();
}
