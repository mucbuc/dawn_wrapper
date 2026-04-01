#pragma once

#include <functional>
#include <iostream>

struct GLFWwindow;

namespace dawn_wrapper {

#define DAWN_WRAPPER_PIMPL_DEC(class_name)   \
private:                                     \
    friend class render_wrapper;             \
    friend class compute_wrapper;            \
    friend class dawn_plugin;                \
    friend class texture_wrapper;            \
    friend class texture_output_wrapper;     \
    friend class bindgroup_wrapper;          \
    friend class buffer_wrapper;             \
    friend class bindgroup_layout_wrapper;   \
    friend class encoder_wrapper;            \
    friend class surface_wrapper;            \
    struct pimpl;                            \
    using ptr_type = std::shared_ptr<pimpl>; \
    class_name(ptr_type);                    \
    ptr_type m_pimpl

struct buffer_wrapper {
    buffer_wrapper() = default;
    buffer_wrapper& write(const std::vector<uint8_t>& colors);
    buffer_wrapper& write(const void*);
    bool done() const;
    buffer_wrapper& get_output(std::function<void(size_t, const void*)>);
    size_t get_size() const;

    bool is_valid() const;
    DAWN_WRAPPER_PIMPL_DEC(buffer_wrapper);
};

struct encoder_wrapper {
    encoder_wrapper() = default;
    encoder_wrapper& submit_command_buffer();
    encoder_wrapper& copy_buffer_to_buffer(buffer_wrapper, buffer_wrapper, size_t offset = 0);
    DAWN_WRAPPER_PIMPL_DEC(encoder_wrapper);
};

struct texture_wrapper {
    texture_wrapper() = default;
    void write(const std::vector<uint8_t>& colors);
    void make_sampler(bool clamp_to_edge);

    bool is_valid() const;
    DAWN_WRAPPER_PIMPL_DEC(texture_wrapper);
};

struct texture_output_wrapper {
    texture_output_wrapper() = default;
    void make_sampler(bool clamp_to_edge);

    bool is_valid() const;
    DAWN_WRAPPER_PIMPL_DEC(texture_output_wrapper);
};

struct bindgroup_layout_wrapper {
    bindgroup_layout_wrapper() = default;
    bindgroup_layout_wrapper& add_buffer(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_read_only_buffer(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_uniform_buffer(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_texture_1d(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_texture_2d(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_storage_texture_2d(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_sampler(unsigned binding, bool enable = true);
    DAWN_WRAPPER_PIMPL_DEC(bindgroup_layout_wrapper);
};

struct bindgroup_wrapper {
    bindgroup_wrapper() = default;
    bindgroup_wrapper& add_buffer(unsigned binding, buffer_wrapper);
    bindgroup_wrapper& add_texture(unsigned binding, texture_wrapper);
    bindgroup_wrapper& add_texture(unsigned binding, texture_output_wrapper);
    bindgroup_wrapper& add_sampler(unsigned binding, texture_wrapper);
    bindgroup_wrapper& add_sampler(unsigned binding, texture_output_wrapper);

    bool is_valid() const;
    DAWN_WRAPPER_PIMPL_DEC(bindgroup_wrapper);
};

struct compute_wrapper {
    compute_wrapper() = default;
    void init_pipeline(bindgroup_layout_wrapper layout);
    void compile_shader(std::string script, std::string entryPoint);
    bool compute(bindgroup_wrapper, unsigned width, unsigned height, encoder_wrapper encoder);
    void setup_compute(unsigned width, unsigned height);
    bindgroup_layout_wrapper make_bindgroup_layout();
    bindgroup_wrapper make_bindgroup();
    bool is_valid() const;
    DAWN_WRAPPER_PIMPL_DEC(compute_wrapper);
};

struct surface_wrapper {

    surface_wrapper() = default;
    void setup(GLFWwindow*, unsigned width, unsigned height, bool opaque);
    void setup(std::string html_canvas_selector, unsigned width, unsigned height);
    void present();
    std::pair<unsigned, unsigned> get_width_and_height() const;

    bool is_valid() const;
    DAWN_WRAPPER_PIMPL_DEC(surface_wrapper);
};

struct render_wrapper {
    render_wrapper() = default;
    void compile_shader(std::string script, std::string entryPoint);
    void set_surface(surface_wrapper);

    void render(bindgroup_wrapper, encoder_wrapper);
    void render(encoder_wrapper);
    bindgroup_layout_wrapper make_bindgroup_layout();
    bindgroup_wrapper make_bindgroup();
    void init_pipeline(bindgroup_layout_wrapper);
    void init_pipeline();
    bool is_valid() const;
    DAWN_WRAPPER_PIMPL_DEC(render_wrapper);
};
#undef DAWN_WRAPPER_PIMPL_DEC

enum class buffer_type {
    storage,
    uniform,
    index,
    vertex,
    map_read,
    copy,
};

struct dawn_plugin {
    dawn_plugin();
    ~dawn_plugin();
    void on_load(std::function<void()>);
    surface_wrapper make_surface();
    render_wrapper make_render();
    compute_wrapper make_compute();
    buffer_wrapper make_src_buffer(size_t size, buffer_type type);
    buffer_wrapper make_dst_buffer(size_t size, buffer_type type);
    texture_wrapper make_texture_1d(size_t);
    texture_wrapper make_texture_2d(size_t, size_t);
    texture_wrapper make_texture_from_data(std::vector<uint8_t> data);
    texture_output_wrapper make_texture_output(size_t, size_t);
    encoder_wrapper make_encoder();
    bool run();
    bool is_valid() const;

private:
    struct dawn_pimpl;
    std::shared_ptr<dawn_pimpl> m_pimpl;
};
}
