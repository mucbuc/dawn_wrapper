
cbuffer cbuffer_u : register(b0) {
  uint4 u[8];
};
RWByteAddressBuffer s : register(u1);
float2x3 v(uint start_byte_offset) {
  float3 v_1 = asfloat(u[(start_byte_offset / 16u)].xyz);
  return float2x3(v_1, asfloat(u[((16u + start_byte_offset) / 16u)].xyz));
}

[numthreads(1, 1, 1)]
void f() {
  float3x2 t = transpose(v(64u));
  float l = length(asfloat(u[1u].xyz).zxy);
  float a = abs(asfloat(u[1u].xyz).zxy.x);
  float v_2 = (t[int(0)].x + float(l));
  s.Store(0u, asuint((v_2 + float(a))));
}

