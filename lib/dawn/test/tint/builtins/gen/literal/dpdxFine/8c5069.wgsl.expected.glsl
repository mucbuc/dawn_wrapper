#version 310 es
precision highp float;
precision highp int;

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  vec4 inner;
} v;
vec4 dpdxFine_8c5069() {
  vec4 res = dFdx(vec4(1.0f));
  return res;
}
void main() {
  v.inner = dpdxFine_8c5069();
}
