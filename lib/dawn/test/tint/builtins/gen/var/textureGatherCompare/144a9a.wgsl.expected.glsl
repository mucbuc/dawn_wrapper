#version 310 es
precision highp float;
precision highp int;

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  vec4 inner;
} v;
uniform highp sampler2DArrayShadow arg_0_arg_1;
vec4 textureGatherCompare_144a9a() {
  vec2 arg_2 = vec2(1.0f);
  uint arg_3 = 1u;
  float arg_4 = 1.0f;
  vec2 v_1 = arg_2;
  float v_2 = arg_4;
  vec4 res = textureGatherOffset(arg_0_arg_1, vec3(v_1, float(arg_3)), v_2, ivec2(1));
  return res;
}
void main() {
  v.inner = textureGatherCompare_144a9a();
}
#version 310 es

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  vec4 inner;
} v;
uniform highp sampler2DArrayShadow arg_0_arg_1;
vec4 textureGatherCompare_144a9a() {
  vec2 arg_2 = vec2(1.0f);
  uint arg_3 = 1u;
  float arg_4 = 1.0f;
  vec2 v_1 = arg_2;
  float v_2 = arg_4;
  vec4 res = textureGatherOffset(arg_0_arg_1, vec3(v_1, float(arg_3)), v_2, ivec2(1));
  return res;
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  v.inner = textureGatherCompare_144a9a();
}
#version 310 es


struct VertexOutput {
  vec4 pos;
  vec4 prevent_dce;
};

uniform highp sampler2DArrayShadow arg_0_arg_1;
layout(location = 0) flat out vec4 vertex_main_loc0_Output;
vec4 textureGatherCompare_144a9a() {
  vec2 arg_2 = vec2(1.0f);
  uint arg_3 = 1u;
  float arg_4 = 1.0f;
  vec2 v = arg_2;
  float v_1 = arg_4;
  vec4 res = textureGatherOffset(arg_0_arg_1, vec3(v, float(arg_3)), v_1, ivec2(1));
  return res;
}
VertexOutput vertex_main_inner() {
  VertexOutput tint_symbol = VertexOutput(vec4(0.0f), vec4(0.0f));
  tint_symbol.pos = vec4(0.0f);
  tint_symbol.prevent_dce = textureGatherCompare_144a9a();
  return tint_symbol;
}
void main() {
  VertexOutput v_2 = vertex_main_inner();
  gl_Position = v_2.pos;
  gl_Position[1u] = -(gl_Position.y);
  gl_Position[2u] = ((2.0f * gl_Position.z) - gl_Position.w);
  vertex_main_loc0_Output = v_2.prevent_dce;
  gl_PointSize = 1.0f;
}
