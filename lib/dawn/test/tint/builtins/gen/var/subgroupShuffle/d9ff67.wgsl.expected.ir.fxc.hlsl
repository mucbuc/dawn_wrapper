SKIP: INVALID


RWByteAddressBuffer prevent_dce : register(u0);
float2 subgroupShuffle_d9ff67() {
  float2 arg_0 = (1.0f).xx;
  int arg_1 = int(1);
  float2 res = WaveReadLaneAt(arg_0, arg_1);
  return res;
}

void fragment_main() {
  prevent_dce.Store2(0u, asuint(subgroupShuffle_d9ff67()));
}

[numthreads(1, 1, 1)]
void compute_main() {
  prevent_dce.Store2(0u, asuint(subgroupShuffle_d9ff67()));
}

FXC validation failure:
<scrubbed_path>(6,16-43): error X3004: undeclared identifier 'WaveReadLaneAt'


tint executable returned error: exit status 1
