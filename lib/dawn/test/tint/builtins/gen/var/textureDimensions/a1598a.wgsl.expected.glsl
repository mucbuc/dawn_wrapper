#version 460
precision highp float;
precision highp int;

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  uvec2 inner;
} v;
uniform highp usamplerCubeArray arg_0;
uvec2 textureDimensions_a1598a() {
  uvec2 res = uvec2(textureSize(arg_0, 0).xy);
  return res;
}
void main() {
  v.inner = textureDimensions_a1598a();
}
#version 460

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  uvec2 inner;
} v;
uniform highp usamplerCubeArray arg_0;
uvec2 textureDimensions_a1598a() {
  uvec2 res = uvec2(textureSize(arg_0, 0).xy);
  return res;
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  v.inner = textureDimensions_a1598a();
}
#version 460


struct VertexOutput {
  vec4 pos;
  uvec2 prevent_dce;
};

uniform highp usamplerCubeArray arg_0;
layout(location = 0) flat out uvec2 vertex_main_loc0_Output;
uvec2 textureDimensions_a1598a() {
  uvec2 res = uvec2(textureSize(arg_0, 0).xy);
  return res;
}
VertexOutput vertex_main_inner() {
  VertexOutput tint_symbol = VertexOutput(vec4(0.0f), uvec2(0u));
  tint_symbol.pos = vec4(0.0f);
  tint_symbol.prevent_dce = textureDimensions_a1598a();
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
