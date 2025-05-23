#version 310 es


struct mat2x2_f32_std140 {
  vec2 col0;
  vec2 col1;
};

layout(binding = 0, std140)
uniform u_block_std140_1_ubo {
  mat2x2_f32_std140 inner[4];
} v;
layout(binding = 1, std430)
buffer s_block_1_ssbo {
  mat2 inner[4];
} v_1;
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  mat2x2_f32_std140 v_2[4] = v.inner;
  mat2 v_3[4] = mat2[4](mat2(vec2(0.0f), vec2(0.0f)), mat2(vec2(0.0f), vec2(0.0f)), mat2(vec2(0.0f), vec2(0.0f)), mat2(vec2(0.0f), vec2(0.0f)));
  {
    uint v_4 = 0u;
    v_4 = 0u;
    while(true) {
      uint v_5 = v_4;
      if ((v_5 >= 4u)) {
        break;
      }
      v_3[v_5] = mat2(v_2[v_5].col0, v_2[v_5].col1);
      {
        v_4 = (v_5 + 1u);
      }
      continue;
    }
  }
  v_1.inner = v_3;
  v_1.inner[1] = mat2(v.inner[2].col0, v.inner[2].col1);
  v_1.inner[1][0] = v.inner[0].col1.yx;
  v_1.inner[1][0][0u] = v.inner[0].col1.x;
}
