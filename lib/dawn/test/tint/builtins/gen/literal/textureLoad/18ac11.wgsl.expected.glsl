#version 460
precision highp float;
precision highp int;

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  ivec4 inner;
} v;
layout(binding = 0, rg32i) uniform highp readonly iimage2D arg_0;
ivec4 textureLoad_18ac11() {
  ivec4 res = imageLoad(arg_0, ivec2(uvec2(1u, 0u)));
  return res;
}
void main() {
  v.inner = textureLoad_18ac11();
}
#version 460

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  ivec4 inner;
} v;
layout(binding = 0, rg32i) uniform highp readonly iimage2D arg_0;
ivec4 textureLoad_18ac11() {
  ivec4 res = imageLoad(arg_0, ivec2(uvec2(1u, 0u)));
  return res;
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  v.inner = textureLoad_18ac11();
}
#version 460


struct VertexOutput {
  vec4 pos;
  ivec4 prevent_dce;
};

layout(binding = 0, rg32i) uniform highp readonly iimage2D arg_0;
layout(location = 0) flat out ivec4 vertex_main_loc0_Output;
ivec4 textureLoad_18ac11() {
  ivec4 res = imageLoad(arg_0, ivec2(uvec2(1u, 0u)));
  return res;
}
VertexOutput vertex_main_inner() {
  VertexOutput tint_symbol = VertexOutput(vec4(0.0f), ivec4(0));
  tint_symbol.pos = vec4(0.0f);
  tint_symbol.prevent_dce = textureLoad_18ac11();
  return tint_symbol;
}
void main() {
  VertexOutput v = vertex_main_inner();
  gl_Position = v.pos;
  gl_Position[1u] = -(gl_Position.y);
  gl_Position[2u] = ((2.0f * gl_Position.z) - gl_Position.w);
  vertex_main_loc0_Output = v.prevent_dce;
  gl_PointSize = 1.0f;
}
