#version 310 es
#extension GL_AMD_gpu_shader_half_float: require

layout(binding = 0, std140)
uniform u_block_std140_1_ubo {
  f16vec3 inner_col0;
  f16vec3 inner_col1;
  f16vec3 inner_col2;
} v;
layout(binding = 1, std430)
buffer s_block_1_ssbo {
  f16mat3 inner;
} v_1;
void tint_store_and_preserve_padding(f16mat3 value_param) {
  v_1.inner[0u] = value_param[0u];
  v_1.inner[1u] = value_param[1u];
  v_1.inner[2u] = value_param[2u];
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  tint_store_and_preserve_padding(f16mat3(v.inner_col0, v.inner_col1, v.inner_col2));
  v_1.inner[1] = f16mat3(v.inner_col0, v.inner_col1, v.inner_col2)[0];
  v_1.inner[1] = f16mat3(v.inner_col0, v.inner_col1, v.inner_col2)[0].zxy;
  v_1.inner[0][1] = f16mat3(v.inner_col0, v.inner_col1, v.inner_col2)[1][0];
}
