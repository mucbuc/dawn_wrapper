#version 310 es

layout(binding = 0, std140)
uniform u_block_1_ubo {
  mat2x4 inner;
} v_1;
void a(mat2x4 m) {
}
void b(vec4 v) {
}
void c(float f_1) {
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  a(v_1.inner);
  b(v_1.inner[1]);
  b(v_1.inner[1].ywxz);
  c(v_1.inner[1].x);
  c(v_1.inner[1].ywxz[0u]);
}
