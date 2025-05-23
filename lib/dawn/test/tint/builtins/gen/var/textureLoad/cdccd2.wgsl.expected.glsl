#version 460
precision highp float;
precision highp int;

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  uvec4 inner;
} v;
layout(binding = 0, rg32ui) uniform highp uimage2DArray arg_0;
uvec4 textureLoad_cdccd2() {
  ivec2 arg_1 = ivec2(1);
  uint arg_2 = 1u;
  uint v_1 = arg_2;
  ivec2 v_2 = ivec2(arg_1);
  uvec4 res = imageLoad(arg_0, ivec3(v_2, int(v_1)));
  return res;
}
void main() {
  v.inner = textureLoad_cdccd2();
}
#version 460

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  uvec4 inner;
} v;
layout(binding = 0, rg32ui) uniform highp uimage2DArray arg_0;
uvec4 textureLoad_cdccd2() {
  ivec2 arg_1 = ivec2(1);
  uint arg_2 = 1u;
  uint v_1 = arg_2;
  ivec2 v_2 = ivec2(arg_1);
  uvec4 res = imageLoad(arg_0, ivec3(v_2, int(v_1)));
  return res;
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  v.inner = textureLoad_cdccd2();
}
