#version 310 es
precision highp float;
precision highp int;

layout(binding = 0, std430)
buffer prevent_dce_block_1_ssbo {
  float inner;
} v;
float dpdxCoarse_029152() {
  float arg_0 = 1.0f;
  float res = dFdx(arg_0);
  return res;
}
void main() {
  v.inner = dpdxCoarse_029152();
}
