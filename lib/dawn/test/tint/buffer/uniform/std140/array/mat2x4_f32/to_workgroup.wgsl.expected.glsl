#version 310 es

layout(binding = 0, std140)
uniform u_block_1_ubo {
  mat2x4 inner[4];
} v;
shared mat2x4 w[4];
void f_inner(uint tint_local_index) {
  {
    uint v_1 = 0u;
    v_1 = tint_local_index;
    while(true) {
      uint v_2 = v_1;
      if ((v_2 >= 4u)) {
        break;
      }
      w[v_2] = mat2x4(vec4(0.0f), vec4(0.0f));
      {
        v_1 = (v_2 + 1u);
      }
      continue;
    }
  }
  barrier();
  w = v.inner;
  w[1] = v.inner[2];
  w[1][0] = v.inner[0][1].ywxz;
  w[1][0][0u] = v.inner[0][1].x;
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  f_inner(gl_LocalInvocationIndex);
}
