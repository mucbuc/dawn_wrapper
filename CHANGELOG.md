- sampler_wrapper: dawn_plugin::make_sampler(sampler_config) with filter_mode
  (linear, nearest) and address_mode (clamp_to_edge, repeat, mirror_repeat);
  bindgroup_wrapper::add_sampler(binding, sampler_wrapper). No texture needed.
- render_config::vertex_from_module: the vertex stage from compile_shader's
  module, so both stages compile once
- render_config::submit: false makes render() only record; the caller submits
  (and natively presents). Defaults to true, as before
- bindgroup_layout_wrapper::set_visibility(binding, shader_visibility): one
  binding's stages, over the layout's
- texture_output_wrapper::write: a texture a compute pass writes can start with
  data
- texture_wrapper::get_width, get_height
- rename dawn_wrapper.h to dawn_wrapper.hpp
- initialize only when setting load complete callback

