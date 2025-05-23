
cbuffer cbuffer_u : register(b0) {
  uint4 u[48];
};
float4x4 v(uint start_byte_offset) {
  float4 v_1 = asfloat(u[(start_byte_offset / 16u)]);
  float4 v_2 = asfloat(u[((16u + start_byte_offset) / 16u)]);
  float4 v_3 = asfloat(u[((32u + start_byte_offset) / 16u)]);
  return float4x4(v_1, v_2, v_3, asfloat(u[((48u + start_byte_offset) / 16u)]));
}

[numthreads(1, 1, 1)]
void f() {
  float4x4 t = transpose(v(400u));
  float l = length(asfloat(u[2u]).ywxz);
  float a = abs(asfloat(u[2u]).ywxz.x);
}

