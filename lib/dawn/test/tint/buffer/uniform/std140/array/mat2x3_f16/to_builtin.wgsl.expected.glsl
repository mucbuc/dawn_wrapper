#version 310 es
#extension GL_AMD_gpu_shader_half_float: require


struct mat2x3_f16_std140 {
  f16vec3 col0;
  f16vec3 col1;
};

layout(binding = 0, std140)
uniform u_block_std140_1_ubo {
  mat2x3_f16_std140 inner[4];
} v;
layout(binding = 1, std430)
buffer s_block_1_ssbo {
  float16_t inner;
} v_1;
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  f16mat3x2 t = transpose(f16mat2x3(v.inner[2].col0, v.inner[2].col1));
  float16_t l = length(v.inner[0].col1.zxy);
  float16_t a = abs(v.inner[0].col1.zxy[0u]);
  float16_t v_2 = float16_t(a);
  v_1.inner = ((v_2 + float16_t(l)) + t[0][0u]);
}
