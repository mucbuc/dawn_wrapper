#version 310 es
#extension GL_AMD_gpu_shader_half_float: require


struct mat2x4_f16_std140 {
  f16vec4 col0;
  f16vec4 col1;
};

layout(binding = 0, std140)
uniform a_block_std140_1_ubo {
  mat2x4_f16_std140 inner[4];
} v;
layout(binding = 1, std430)
buffer s_block_1_ssbo {
  float16_t inner;
} v_1;
int counter = 0;
int i() {
  counter = (counter + 1);
  return counter;
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  int v_2 = i();
  f16mat2x4 v_3 = f16mat2x4(v.inner[v_2].col0, v.inner[v_2].col1);
  f16vec4 v_4 = v_3[i()];
  mat2x4_f16_std140 v_5[4] = v.inner;
  f16mat2x4 v_6[4] = f16mat2x4[4](f16mat2x4(f16vec4(0.0hf), f16vec4(0.0hf)), f16mat2x4(f16vec4(0.0hf), f16vec4(0.0hf)), f16mat2x4(f16vec4(0.0hf), f16vec4(0.0hf)), f16mat2x4(f16vec4(0.0hf), f16vec4(0.0hf)));
  {
    uint v_7 = 0u;
    v_7 = 0u;
    while(true) {
      uint v_8 = v_7;
      if ((v_8 >= 4u)) {
        break;
      }
      v_6[v_8] = f16mat2x4(v_5[v_8].col0, v_5[v_8].col1);
      {
        v_7 = (v_8 + 1u);
      }
      continue;
    }
  }
  f16mat2x4 l_a[4] = v_6;
  f16mat2x4 l_a_i = v_3;
  f16vec4 l_a_i_i = v_4;
  v_1.inner = (((v_4[0u] + l_a[0][0][0u]) + l_a_i[0][0u]) + l_a_i_i[0u]);
}
