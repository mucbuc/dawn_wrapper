#pragma once

#include <array>
#include <functional>
#include <iostream>

struct GLFWwindow;
namespace dawn_wrapper {

#define WRAPPER_PIMPL_DEC(class_name)        \
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
    struct pimpl;                            \
    using ptr_type = std::shared_ptr<pimpl>; \
    class_name(ptr_type);                    \
    ptr_type m_pimpl

struct buffer_wrapper {
    buffer_wrapper() = default;
    buffer_wrapper& write(const std::vector<uint8_t>& colors);
    buffer_wrapper& write(const void*);
    bool done();
    buffer_wrapper& get_output(std::function<void(unsigned, const void*)>);
    unsigned long get_size();

    operator bool() const;
    WRAPPER_PIMPL_DEC(buffer_wrapper);
};

struct encoder_wrapper {
    encoder_wrapper() = default;
    encoder_wrapper& submit_command_buffer();
    encoder_wrapper& copy_buffer_to_buffer(buffer_wrapper, buffer_wrapper, size_t offset = 0);
    WRAPPER_PIMPL_DEC(encoder_wrapper);
};

struct texture_wrapper {
    texture_wrapper() = default;
    void write(const std::vector<uint8_t>& colors);
    void make_sampler(bool clamp_to_edge);

    operator bool() const;
    WRAPPER_PIMPL_DEC(texture_wrapper);
};

struct texture_output_wrapper {
    texture_output_wrapper() = default;
    void make_sampler(bool clamp_to_edge);
    WRAPPER_PIMPL_DEC(texture_output_wrapper);
};

struct bindgroup_layout_wrapper {
    bindgroup_layout_wrapper() = default;
    bindgroup_layout_wrapper& add_buffer(unsigned binding);
    bindgroup_layout_wrapper& add_read_only_buffer(unsigned binding);
    bindgroup_layout_wrapper& add_uniform_buffer(unsigned binding);
    bindgroup_layout_wrapper& add_texture_1d(unsigned binding);
    bindgroup_layout_wrapper& add_texture_2d(unsigned binding);
    bindgroup_layout_wrapper& add_storage_texture_2d(unsigned binding);
    bindgroup_layout_wrapper& add_sampler(unsigned binding);
    WRAPPER_PIMPL_DEC(bindgroup_layout_wrapper);
};

struct bindgroup_wrapper {
    bindgroup_wrapper() = default;
    bindgroup_wrapper& add_buffer(unsigned binding, buffer_wrapper);
    bindgroup_wrapper& addTexture(unsigned binding, texture_wrapper);
    bindgroup_wrapper& addTexture(unsigned binding, texture_output_wrapper);
    bindgroup_wrapper& add_sampler(unsigned binding, texture_wrapper);
    bindgroup_wrapper& add_sampler(unsigned binding, texture_output_wrapper);

    operator bool() const;
    WRAPPER_PIMPL_DEC(bindgroup_wrapper);
};

struct compute_wrapper {
    compute_wrapper() = default;
    void init_pipeline(bindgroup_layout_wrapper layout);
    void compile_shader(std::string script, std::string entryPoint);
    bool compute(bindgroup_wrapper, unsigned width, unsigned height, encoder_wrapper encoder);
    void setup_compute(unsigned width, unsigned height);
    bindgroup_layout_wrapper make_bindgroup_layout();
    bindgroup_wrapper make_bindgroup();
    operator bool() const;
    WRAPPER_PIMPL_DEC(compute_wrapper);
};

struct render_wrapper {
    render_wrapper() = default;
    void compile_shader(std::string script, std::string entryPoint);
    void setup_surface(GLFWwindow*, unsigned width, unsigned height, bool opaque);
    void setup_surface_html_canvas(std::string selector, unsigned width, unsigned height);
    void render(bindgroup_wrapper, encoder_wrapper);
    void render(encoder_wrapper);
    bindgroup_layout_wrapper make_bindgroup_layout();
    bindgroup_wrapper make_bindgroup();
    void init_pipeline(bindgroup_layout_wrapper);
    void init_pipeline();
    operator bool() const;
    WRAPPER_PIMPL_DEC(render_wrapper);
};
#undef WRAPPER_PIMPL_DEC

enum class buffer_type {
    storage,
    uniform,
    index,
    vertex,
    map_read,
};

struct dawn_plugin {
    dawn_plugin();
    ~dawn_plugin();
    render_wrapper make_render();
    compute_wrapper make_compute();
    buffer_wrapper make_src_buffer(unsigned size, buffer_type type);
    buffer_wrapper make_dst_buffer(unsigned size, buffer_type type);
    texture_wrapper make_texture(unsigned);
    texture_wrapper make_texture(unsigned, unsigned);
    texture_wrapper make_texture(std::vector<uint8_t> data);
    texture_output_wrapper make_texture_output(unsigned, unsigned);
    encoder_wrapper make_encoder();
    bool run();
    operator bool() const;

private:
    struct dawn_pimpl;
    std::shared_ptr<dawn_pimpl> m_pimpl;
};
}
