#version 310 es


struct S {
  int before;
  uint tint_pad_0;
  uint tint_pad_1;
  uint tint_pad_2;
  mat2x4 m;
  uint tint_pad_3;
  uint tint_pad_4;
  uint tint_pad_5;
  uint tint_pad_6;
  int after;
  uint tint_pad_7;
  uint tint_pad_8;
  uint tint_pad_9;
  uint tint_pad_10;
  uint tint_pad_11;
  uint tint_pad_12;
  uint tint_pad_13;
  uint tint_pad_14;
  uint tint_pad_15;
  uint tint_pad_16;
  uint tint_pad_17;
  uint tint_pad_18;
  uint tint_pad_19;
  uint tint_pad_20;
  uint tint_pad_21;
};

layout(binding = 0, std140)
uniform u_block_1_ubo {
  S inner[4];
} v;
S p[4] = S[4](S(0, 0u, 0u, 0u, mat2x4(vec4(0.0f), vec4(0.0f)), 0u, 0u, 0u, 0u, 0, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u), S(0, 0u, 0u, 0u, mat2x4(vec4(0.0f), vec4(0.0f)), 0u, 0u, 0u, 0u, 0, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u), S(0, 0u, 0u, 0u, mat2x4(vec4(0.0f), vec4(0.0f)), 0u, 0u, 0u, 0u, 0, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u), S(0, 0u, 0u, 0u, mat2x4(vec4(0.0f), vec4(0.0f)), 0u, 0u, 0u, 0u, 0, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u));
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  p = v.inner;
  p[1] = v.inner[2];
  p[3].m = v.inner[2].m;
  p[1].m[0] = v.inner[0].m[1].ywxz;
}
