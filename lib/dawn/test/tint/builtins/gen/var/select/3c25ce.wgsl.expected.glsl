#version 310 es
precision highp float;
precision highp int;

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  int inner;
} v;
int select_3c25ce() {
  bvec3 arg_0 = bvec3(true);
  bvec3 arg_1 = bvec3(true);
  bool arg_2 = true;
  bvec3 v_1 = arg_0;
  bvec3 v_2 = arg_1;
  bvec3 res = mix(v_1, v_2, bvec3(arg_2));
  return mix(0, 1, all(equal(res, bvec3(false))));
}
void main() {
  v.inner = select_3c25ce();
}
#version 310 es

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  int inner;
} v;
int select_3c25ce() {
  bvec3 arg_0 = bvec3(true);
  bvec3 arg_1 = bvec3(true);
  bool arg_2 = true;
  bvec3 v_1 = arg_0;
  bvec3 v_2 = arg_1;
  bvec3 res = mix(v_1, v_2, bvec3(arg_2));
  return mix(0, 1, all(equal(res, bvec3(false))));
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  v.inner = select_3c25ce();
}
#version 310 es


struct VertexOutput {
  vec4 pos;
  int prevent_dce;
};

layout(location = 0) flat out int vertex_main_loc0_Output;
int select_3c25ce() {
  bvec3 arg_0 = bvec3(true);
  bvec3 arg_1 = bvec3(true);
  bool arg_2 = true;
  bvec3 v = arg_0;
  bvec3 v_1 = arg_1;
  bvec3 res = mix(v, v_1, bvec3(arg_2));
  return mix(0, 1, all(equal(res, bvec3(false))));
}
VertexOutput vertex_main_inner() {
  VertexOutput tint_symbol = VertexOutput(vec4(0.0f), 0);
  tint_symbol.pos = vec4(0.0f);
  tint_symbol.prevent_dce = select_3c25ce();
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
