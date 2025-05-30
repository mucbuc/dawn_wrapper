#version 310 es
precision highp float;
precision highp int;

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  ivec4 inner;
} v;
uniform highp isampler2D arg_1_arg_2;
ivec4 textureGather_bb3ac5() {
  vec2 arg_3 = vec2(1.0f);
  vec2 v_1 = arg_3;
  ivec4 res = textureGather(arg_1_arg_2, v_1, int(1));
  return res;
}
void main() {
  v.inner = textureGather_bb3ac5();
}
#version 310 es

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  ivec4 inner;
} v;
uniform highp isampler2D arg_1_arg_2;
ivec4 textureGather_bb3ac5() {
  vec2 arg_3 = vec2(1.0f);
  vec2 v_1 = arg_3;
  ivec4 res = textureGather(arg_1_arg_2, v_1, int(1));
  return res;
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  v.inner = textureGather_bb3ac5();
}
#version 310 es


struct VertexOutput {
  vec4 pos;
  ivec4 prevent_dce;
};

uniform highp isampler2D arg_1_arg_2;
layout(location = 0) flat out ivec4 vertex_main_loc0_Output;
ivec4 textureGather_bb3ac5() {
  vec2 arg_3 = vec2(1.0f);
  vec2 v = arg_3;
  ivec4 res = textureGather(arg_1_arg_2, v, int(1));
  return res;
}
VertexOutput vertex_main_inner() {
  VertexOutput tint_symbol = VertexOutput(vec4(0.0f), ivec4(0));
  tint_symbol.pos = vec4(0.0f);
  tint_symbol.prevent_dce = textureGather_bb3ac5();
  return tint_symbol;
}
void main() {
  VertexOutput v_1 = vertex_main_inner();
  gl_Position = v_1.pos;
  gl_Position[1u] = -(gl_Position.y);
  gl_Position[2u] = ((2.0f * gl_Position.z) - gl_Position.w);
  vertex_main_loc0_Output = v_1.prevent_dce;
  gl_PointSize = 1.0f;
}
