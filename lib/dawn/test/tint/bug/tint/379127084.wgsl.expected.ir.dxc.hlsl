struct FSIn {
  uint2 ssboIndicesVar;
  float2 localCoordsVar;
};

struct FSOut {
  float4 sk_FragColor;
};

struct main_outputs {
  float4 FSOut_sk_FragColor : SV_Target0;
};

struct main_inputs {
  nointerpolation uint2 FSIn_ssboIndicesVar : TEXCOORD0;
  float2 FSIn_localCoordsVar : TEXCOORD1;
};


cbuffer cbuffer__uniform0 : register(b0) {
  uint4 _uniform0[2];
};
ByteAddressBuffer _storage1 : register(t2);
static uint shadingSsboIndex = 0u;
SamplerState permutationsSampler_1_Sampler : register(s0, space1);
Texture2D<float4> permutationsSampler_1_Texture : register(t1, space1);
SamplerState noiseSampler_1_Sampler : register(s2, space1);
Texture2D<float4> noiseSampler_1_Texture : register(t3, space1);
float4x4 v(uint offset) {
  float4 v_1 = asfloat(_storage1.Load4((offset + 0u)));
  float4 v_2 = asfloat(_storage1.Load4((offset + 16u)));
  float4 v_3 = asfloat(_storage1.Load4((offset + 32u)));
  return float4x4(v_1, v_2, v_3, asfloat(_storage1.Load4((offset + 48u))));
}

void _skslMain(FSIn _stageIn, inout FSOut _stageOut) {
  shadingSsboIndex = _stageIn.ssboIndicesVar.y;
  int _56_d = asint(_storage1.Load((16u + (uint(shadingSsboIndex) * 128u))));
  float2 _57_k = float2(((_stageIn.localCoordsVar + 0.5f) * asfloat(_storage1.Load2((0u + (uint(shadingSsboIndex) * 128u))))));
  float4 _58_l = (0.0f).xxxx;
  float2 _59_m = float2(asfloat(_storage1.Load2((8u + (uint(shadingSsboIndex) * 128u)))));
  float _60_n = 1.0f;
  int _61_o = int(0);
  {
    while(true) {
      int v_4 = _61_o;
      if ((v_4 < asint(_storage1.Load((20u + (uint(shadingSsboIndex) * 128u)))))) {
        float4 _62_f = (0.0f).xxxx;
        float2 _skTemp2 = floor(_57_k);
        _62_f = float4(_skTemp2, _62_f.zw);
        _62_f = float4(_62_f.xy, (_62_f.xy + (1.0f).xx));
        if (bool(asint(_storage1.Load((24u + (uint(shadingSsboIndex) * 128u)))))) {
          float4 _skTemp3 = step(_59_m.xyxy, _62_f);
          _62_f = (_62_f - (_skTemp3 * _59_m.xyxy));
        }
        float2 v_5 = float2(float2(((_62_f.x + 0.5f) * 0.00390625f), 0.5f));
        float _63_g = permutationsSampler_1_Texture.SampleBias(permutationsSampler_1_Sampler, v_5, clamp(-0.47499999403953552246f, -16.0f, 15.9899997711181640625f)).x;
        float2 v_6 = float2(float2(((_62_f.z + 0.5f) * 0.00390625f), 0.5f));
        float _64_h = permutationsSampler_1_Texture.SampleBias(permutationsSampler_1_Sampler, v_6, clamp(-0.47499999403953552246f, -16.0f, 15.9899997711181640625f)).x;
        float2 _65_i = float2(_63_g, _64_h);
        if (false) {
          float2 _skTemp4 = floor(((_65_i * (255.0f).xx) + (0.5f).xx));
          _65_i = (_skTemp4 * (0.0039215688593685627f).xx);
        }
        float4 _66_j = ((256.0f * _65_i.xyxy) + _62_f.yyww);
        _66_j = (_66_j * (0.00390625f).xxxx);
        float4 _67_p = _66_j;
        float2 _skTemp5 = frac(_57_k);
        float2 _68_d = _skTemp5;
        float2 _skTemp6 = smoothstep((0.0f).xx, (1.0f).xx, _68_d);
        float2 _69_e = _skTemp6;
        float4 _71_g = (0.0f).xxxx;
        int _72_h = int(0);
        {
          while(true) {
            float _73_i = ((float(_72_h) + 0.5f) * 0.25f);
            float v_7 = float(_67_p.x);
            float2 v_8 = float2(v_7, float(_73_i));
            float4 _74_j = noiseSampler_1_Texture.SampleBias(noiseSampler_1_Sampler, v_8, clamp(-0.47499999403953552246f, -16.0f, 15.9899997711181640625f));
            float v_9 = float(_67_p.y);
            float2 v_10 = float2(v_9, float(_73_i));
            float4 _75_k = noiseSampler_1_Texture.SampleBias(noiseSampler_1_Sampler, v_10, clamp(-0.47499999403953552246f, -16.0f, 15.9899997711181640625f));
            float v_11 = float(_67_p.w);
            float2 v_12 = float2(v_11, float(_73_i));
            float4 _76_l = noiseSampler_1_Texture.SampleBias(noiseSampler_1_Sampler, v_12, clamp(-0.47499999403953552246f, -16.0f, 15.9899997711181640625f));
            float v_13 = float(_67_p.z);
            float2 v_14 = float2(v_13, float(_73_i));
            float4 _77_m = noiseSampler_1_Texture.SampleBias(noiseSampler_1_Sampler, v_14, clamp(-0.47499999403953552246f, -16.0f, 15.9899997711181640625f));
            float2 _78_n = _68_d;
            float _skTemp7 = dot((((_74_j.yw + (_74_j.xz * 0.00390625f)) * 2.0f) - 1.0f), _78_n);
            float _79_o = _skTemp7;
            _78_n.x = (_78_n.x - 1.0f);
            float _skTemp8 = dot((((_75_k.yw + (_75_k.xz * 0.00390625f)) * 2.0f) - 1.0f), _78_n);
            float _80_p = _skTemp8;
            float _skTemp9 = lerp(_79_o, _80_p, _69_e.x);
            float _81_q = _skTemp9;
            _78_n.y = (_78_n.y - 1.0f);
            float _skTemp10 = dot((((_76_l.yw + (_76_l.xz * 0.00390625f)) * 2.0f) - 1.0f), _78_n);
            _80_p = _skTemp10;
            _78_n.x = (_78_n.x + 1.0f);
            float _skTemp11 = dot((((_77_m.yw + (_77_m.xz * 0.00390625f)) * 2.0f) - 1.0f), _78_n);
            _79_o = _skTemp11;
            float _skTemp12 = lerp(_79_o, _80_p, _69_e.x);
            float _82_r = _skTemp12;
            float _skTemp13 = lerp(_81_q, _82_r, _69_e.y);
            _71_g[_72_h] = _skTemp13;
            {
              _72_h = (_72_h + int(1));
              if ((_72_h >= int(4))) { break; }
            }
            continue;
          }
        }
        float4 _83_q = _71_g;
        if ((_56_d != int(0))) {
          float4 _skTemp14 = abs(_83_q);
          _83_q = _skTemp14;
        }
        _58_l = (_58_l + (_83_q * _60_n));
        _57_k = (_57_k * (2.0f).xx);
        _60_n = (_60_n * 0.5f);
        _59_m = (_59_m * (2.0f).xx);
      } else {
        break;
      }
      {
        _61_o = (_61_o + int(1));
      }
      continue;
    }
  }
  if ((_56_d == int(0))) {
    _58_l = ((_58_l * (0.5f).xxxx) + (0.5f).xxxx);
  }
  float4 _skTemp15 = saturate(_58_l);
  _58_l = _skTemp15;
  float3 v_15 = float3(_58_l.xyz);
  float3 v_16 = float3((v_15 * float(_58_l.w)));
  float _skTemp16 = dot(float3(0.21259999275207519531f, 0.71520000696182250977f, 0.07220000028610229492f), float4(v_16, float(float(_58_l.w))).xyz);
  float _skTemp17 = saturate(_skTemp16);
  float4 _84_a = float4(0.0f, 0.0f, 0.0f, _skTemp17);
  int _85_d = asint(_storage1.Load((112u + (uint(shadingSsboIndex) * 128u))));
  if (bool(_85_d)) {
    float4 _skTemp18 = (0.0f).xxxx;
    if ((_84_a.y < _84_a.z)) {
      _skTemp18 = float4(_84_a.zy, -1.0f, 0.6666666865348815918f);
    } else {
      _skTemp18 = float4(_84_a.yz, 0.0f, -0.3333333432674407959f);
    }
    float4 _86_e = _skTemp18;
    float4 _skTemp19 = (0.0f).xxxx;
    if ((_84_a.x < _86_e.x)) {
      _skTemp19 = float4(_86_e.x, _84_a.x, _86_e.yw);
    } else {
      _skTemp19 = float4(_84_a.x, _86_e.x, _86_e.yz);
    }
    float4 _87_f = _skTemp19;
    float _88_h = _87_f.x;
    float _skTemp20 = min(_87_f.y, _87_f.z);
    float _89_i = (_88_h - _skTemp20);
    float _90_j = (_88_h - (_89_i * 0.5f));
    float _skTemp21 = abs((_87_f.w + ((_87_f.y - _87_f.z) / ((_89_i * 6.0f) + 0.00009999999747378752f))));
    float _91_k = _skTemp21;
    float _skTemp22 = abs(((_90_j * 2.0f) - _84_a.w));
    float _92_l = (_89_i / ((_84_a.w + 0.00009999999747378752f) - _skTemp22));
    float _93_m = (_90_j / (_84_a.w + 0.00009999999747378752f));
    _84_a = float4(_91_k, _92_l, _93_m, _84_a.w);
  } else {
    float _skTemp23 = max(_84_a.w, 0.00009999999747378752f);
    _84_a = float4((_84_a.xyz / _skTemp23), _84_a.w);
  }
  float4x4 v_17 = v((32u + (uint(shadingSsboIndex) * 128u)));
  float4 v_18 = mul(float4(_84_a), v_17);
  float4 _94_f = float4((v_18 + asfloat(_storage1.Load4((96u + (uint(shadingSsboIndex) * 128u))))));
  if (bool(_85_d)) {
    float _skTemp24 = abs(((2.0f * _94_f.z) - 1.0f));
    float _95_b = ((1.0f - _skTemp24) * _94_f.y);
    float3 _96_c = (_94_f.xxx + float3(0.0f, 0.6666666865348815918f, 0.3333333432674407959f));
    float3 _skTemp25 = frac(_96_c);
    float3 _skTemp26 = abs(((_skTemp25 * 6.0f) - 3.0f));
    float3 _skTemp27 = saturate((_skTemp26 - 1.0f));
    float3 _97_d = _skTemp27;
    float4 _skTemp28 = saturate(float4(((((_97_d - 0.5f) * _95_b) + _94_f.z) * _94_f.w), _94_f.w));
    _94_f = _skTemp28;
  } else {
    if (bool(asint(_storage1.Load((116u + (uint(shadingSsboIndex) * 128u)))))) {
      float4 _skTemp29 = saturate(_94_f);
      _94_f = _skTemp29;
    } else {
      float _skTemp30 = saturate(_94_f.w);
      _94_f.w = _skTemp30;
    }
    _94_f = float4((_94_f.xyz * _94_f.w), _94_f.w);
  }
  float4 outColor_0 = _94_f;
  _stageOut.sk_FragColor = outColor_0;
}

FSOut main_inner(FSIn _stageIn) {
  FSOut _stageOut = (FSOut)0;
  _skslMain(_stageIn, _stageOut);
  FSOut v_19 = _stageOut;
  return v_19;
}

main_outputs main(main_inputs inputs) {
  FSIn v_20 = {inputs.FSIn_ssboIndicesVar, inputs.FSIn_localCoordsVar};
  FSOut v_21 = main_inner(v_20);
  main_outputs v_22 = {v_21.sk_FragColor};
  return v_22;
}

