struct VertexOutput {
  float4 pos;
  int3 prevent_dce;
};

struct vertex_main_outputs {
  nointerpolation int3 VertexOutput_prevent_dce : TEXCOORD0;
  float4 VertexOutput_pos : SV_Position;
};


RWByteAddressBuffer prevent_dce : register(u0);
int3 countLeadingZeros_7c38a6() {
  int3 arg_0 = (int(1)).xxx;
  uint3 v = asuint(arg_0);
  uint3 v_1 = (((v <= (65535u).xxx)) ? ((16u).xxx) : ((0u).xxx));
  uint3 v_2 = ((((v << v_1) <= (16777215u).xxx)) ? ((8u).xxx) : ((0u).xxx));
  uint3 v_3 = (((((v << v_1) << v_2) <= (268435455u).xxx)) ? ((4u).xxx) : ((0u).xxx));
  uint3 v_4 = ((((((v << v_1) << v_2) << v_3) <= (1073741823u).xxx)) ? ((2u).xxx) : ((0u).xxx));
  uint3 v_5 = (((((((v << v_1) << v_2) << v_3) << v_4) <= (2147483647u).xxx)) ? ((1u).xxx) : ((0u).xxx));
  uint3 v_6 = (((((((v << v_1) << v_2) << v_3) << v_4) == (0u).xxx)) ? ((1u).xxx) : ((0u).xxx));
  int3 res = asint(((v_1 | (v_2 | (v_3 | (v_4 | (v_5 | v_6))))) + v_6));
  return res;
}

void fragment_main() {
  prevent_dce.Store3(0u, asuint(countLeadingZeros_7c38a6()));
}

[numthreads(1, 1, 1)]
void compute_main() {
  prevent_dce.Store3(0u, asuint(countLeadingZeros_7c38a6()));
}

VertexOutput vertex_main_inner() {
  VertexOutput tint_symbol = (VertexOutput)0;
  tint_symbol.pos = (0.0f).xxxx;
  tint_symbol.prevent_dce = countLeadingZeros_7c38a6();
  VertexOutput v_7 = tint_symbol;
  return v_7;
}

vertex_main_outputs vertex_main() {
  VertexOutput v_8 = vertex_main_inner();
  vertex_main_outputs v_9 = {v_8.prevent_dce, v_8.pos};
  return v_9;
}

