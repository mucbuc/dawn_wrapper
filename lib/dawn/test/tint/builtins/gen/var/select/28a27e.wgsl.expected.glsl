#version 310 es
precision highp float;
precision highp int;

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  uvec3 inner;
} v;
uvec3 select_28a27e() {
  uvec3 arg_0 = uvec3(1u);
  uvec3 arg_1 = uvec3(1u);
  bvec3 arg_2 = bvec3(true);
  uvec3 res = mix(arg_0, arg_1, arg_2);
  return res;
}
void main() {
  v.inner = select_28a27e();
}
#version 310 es

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  uvec3 inner;
} v;
uvec3 select_28a27e() {
  uvec3 arg_0 = uvec3(1u);
  uvec3 arg_1 = uvec3(1u);
  bvec3 arg_2 = bvec3(true);
  uvec3 res = mix(arg_0, arg_1, arg_2);
  return res;
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  v.inner = select_28a27e();
}
#version 310 es


struct VertexOutput {
  vec4 pos;
  uvec3 prevent_dce;
};

layout(location = 0) flat out uvec3 vertex_main_loc0_Output;
uvec3 select_28a27e() {
  uvec3 arg_0 = uvec3(1u);
  uvec3 arg_1 = uvec3(1u);
  bvec3 arg_2 = bvec3(true);
  uvec3 res = mix(arg_0, arg_1, arg_2);
  return res;
}
VertexOutput vertex_main_inner() {
  VertexOutput tint_symbol = VertexOutput(vec4(0.0f), uvec3(0u));
  tint_symbol.pos = vec4(0.0f);
  tint_symbol.prevent_dce = select_28a27e();
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
