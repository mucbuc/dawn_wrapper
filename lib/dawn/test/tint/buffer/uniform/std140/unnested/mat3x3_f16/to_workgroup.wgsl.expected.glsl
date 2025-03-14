#version 310 es
#extension GL_AMD_gpu_shader_half_float: require

layout(binding = 0, std140)
uniform u_block_std140_1_ubo {
  f16vec3 inner_col0;
  f16vec3 inner_col1;
  f16vec3 inner_col2;
} v;
shared f16mat3 w;
void f_inner(uint tint_local_index) {
  if ((tint_local_index == 0u)) {
    w = f16mat3(f16vec3(0.0hf), f16vec3(0.0hf), f16vec3(0.0hf));
  }
  barrier();
  w = f16mat3(v.inner_col0, v.inner_col1, v.inner_col2);
  w[1] = f16mat3(v.inner_col0, v.inner_col1, v.inner_col2)[0];
  w[1] = f16mat3(v.inner_col0, v.inner_col1, v.inner_col2)[0].zxy;
  w[0][1] = f16mat3(v.inner_col0, v.inner_col1, v.inner_col2)[1][0];
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  f_inner(gl_LocalInvocationIndex);
}
