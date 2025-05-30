#version 310 es
precision highp float;
precision highp int;

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  float inner;
} v;
float step_0b073b() {
  float arg_0 = 1.0f;
  float arg_1 = 1.0f;
  float res = step(arg_0, arg_1);
  return res;
}
void main() {
  v.inner = step_0b073b();
}
#version 310 es

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  float inner;
} v;
float step_0b073b() {
  float arg_0 = 1.0f;
  float arg_1 = 1.0f;
  float res = step(arg_0, arg_1);
  return res;
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  v.inner = step_0b073b();
}
#version 310 es


struct VertexOutput {
  vec4 pos;
  float prevent_dce;
};

layout(location = 0) flat out float vertex_main_loc0_Output;
float step_0b073b() {
  float arg_0 = 1.0f;
  float arg_1 = 1.0f;
  float res = step(arg_0, arg_1);
  return res;
}
VertexOutput vertex_main_inner() {
  VertexOutput tint_symbol = VertexOutput(vec4(0.0f), 0.0f);
  tint_symbol.pos = vec4(0.0f);
  tint_symbol.prevent_dce = step_0b073b();
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
