#version 310 es
#extension GL_AMD_gpu_shader_half_float: require


struct tint_push_constant_struct {
  uint tint_first_instance;
};

struct VertexInputs0 {
  uint vertex_index;
  int loc0;
};

struct VertexInputs1 {
  float loc2;
  vec4 loc3;
  f16vec3 loc5;
};

layout(location = 0) uniform tint_push_constant_struct tint_push_constants;
layout(location = 0) in int tint_symbol_loc0_Input;
layout(location = 1) in uint tint_symbol_loc1_Input;
layout(location = 2) in float tint_symbol_loc2_Input;
layout(location = 3) in vec4 tint_symbol_loc3_Input;
layout(location = 5) in f16vec3 tint_symbol_loc5_Input;
layout(location = 4) in float16_t tint_symbol_loc4_Input;
vec4 tint_symbol_inner(VertexInputs0 inputs0, uint loc1, uint instance_index, VertexInputs1 inputs1, float16_t loc4) {
  uint foo = (inputs0.vertex_index + instance_index);
  int i = inputs0.loc0;
  uint u = loc1;
  float f = inputs1.loc2;
  vec4 v = inputs1.loc3;
  float16_t x = loc4;
  f16vec3 y = inputs1.loc5;
  return vec4(0.0f);
}
void main() {
  uint v_1 = uint(gl_VertexID);
  VertexInputs0 v_2 = VertexInputs0(v_1, tint_symbol_loc0_Input);
  uint v_3 = tint_symbol_loc1_Input;
  uint v_4 = uint(gl_InstanceID);
  uint v_5 = (v_4 + tint_push_constants.tint_first_instance);
  VertexInputs1 v_6 = VertexInputs1(tint_symbol_loc2_Input, tint_symbol_loc3_Input, tint_symbol_loc5_Input);
  gl_Position = tint_symbol_inner(v_2, v_3, v_5, v_6, tint_symbol_loc4_Input);
  gl_Position[1u] = -(gl_Position.y);
  gl_Position[2u] = ((2.0f * gl_Position.z) - gl_Position.w);
  gl_PointSize = 1.0f;
}
