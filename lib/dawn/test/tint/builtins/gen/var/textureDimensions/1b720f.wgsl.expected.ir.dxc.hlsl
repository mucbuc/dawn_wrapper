struct VertexOutput {
  float4 pos;
  uint3 prevent_dce;
};

struct vertex_main_outputs {
  nointerpolation uint3 VertexOutput_prevent_dce : TEXCOORD0;
  float4 VertexOutput_pos : SV_Position;
};


RWByteAddressBuffer prevent_dce : register(u0);
Texture3D<int4> arg_0 : register(t0, space1);
uint3 textureDimensions_1b720f() {
  uint3 v = (0u).xxx;
  arg_0.GetDimensions(v.x, v.y, v.z);
  uint3 res = v;
  return res;
}

void fragment_main() {
  prevent_dce.Store3(0u, textureDimensions_1b720f());
}

[numthreads(1, 1, 1)]
void compute_main() {
  prevent_dce.Store3(0u, textureDimensions_1b720f());
}

VertexOutput vertex_main_inner() {
  VertexOutput tint_symbol = (VertexOutput)0;
  tint_symbol.pos = (0.0f).xxxx;
  tint_symbol.prevent_dce = textureDimensions_1b720f();
  VertexOutput v_1 = tint_symbol;
  return v_1;
}

vertex_main_outputs vertex_main() {
  VertexOutput v_2 = vertex_main_inner();
  vertex_main_outputs v_3 = {v_2.prevent_dce, v_2.pos};
  return v_3;
}

