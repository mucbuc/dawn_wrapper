#version 310 es


struct UBO {
  int dynamic_idx;
};

struct Result {
  int tint_symbol;
};

struct S {
  int data[64];
};

layout(binding = 0, std140)
uniform ubo_block_1_ubo {
  UBO inner;
} v;
layout(binding = 1, std430)
buffer result_block_1_ssbo {
  Result inner;
} v_1;
shared S s;
void f_inner(uint tint_local_index) {
  {
    uint v_2 = 0u;
    v_2 = tint_local_index;
    while(true) {
      uint v_3 = v_2;
      if ((v_3 >= 64u)) {
        break;
      }
      s.data[v_3] = 0;
      {
        v_2 = (v_3 + 1u);
      }
      continue;
    }
  }
  barrier();
  int v_4 = v.inner.dynamic_idx;
  s.data[v_4] = 1;
  v_1.inner.tint_symbol = s.data[3];
}
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  f_inner(gl_LocalInvocationIndex);
}
