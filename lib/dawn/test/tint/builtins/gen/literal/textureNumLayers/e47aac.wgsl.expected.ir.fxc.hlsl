
RWByteAddressBuffer prevent_dce : register(u0);
RWTexture2DArray<float4> arg_0 : register(u0, space1);
uint textureNumLayers_e47aac() {
  uint3 v = (0u).xxx;
  arg_0.GetDimensions(v.x, v.y, v.z);
  uint res = v.z;
  return res;
}

void fragment_main() {
  prevent_dce.Store(0u, textureNumLayers_e47aac());
}

[numthreads(1, 1, 1)]
void compute_main() {
  prevent_dce.Store(0u, textureNumLayers_e47aac());
}

