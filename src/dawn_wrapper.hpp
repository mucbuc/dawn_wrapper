#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>

struct GLFWwindow;

namespace dawn_wrapper {

// Shared pimpl boilerplate — only the struct/type/constructor declarations.
// Friend declarations are listed explicitly per class below.
#define DAWN_WRAPPER_PIMPL_CORE(class_name)  \
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

private:
    // dawn_plugin: constructs via ptr_type constructor
    // encoder_wrapper: accesses m_pimpl->m_buffer in copy_buffer_to_buffer
    // bindgroup_wrapper: accesses m_pimpl->m_buffer in add_buffer
    friend class dawn_plugin;
    friend class encoder_wrapper;
    friend class bindgroup_wrapper;
    DAWN_WRAPPER_PIMPL_CORE(buffer_wrapper);
};

struct encoder_wrapper {
    encoder_wrapper() = default;
    encoder_wrapper& submit_command_buffer();
    encoder_wrapper& copy_buffer_to_buffer(buffer_wrapper, buffer_wrapper, size_t offset = 0);

private:
    // dawn_plugin: constructs via ptr_type constructor
    // compute_wrapper: accesses m_pimpl->m_encoder in compute()
    // render_wrapper: accesses m_pimpl->m_encoder in render()
    friend class dawn_plugin;
    friend class compute_wrapper;
    friend class render_wrapper;
    DAWN_WRAPPER_PIMPL_CORE(encoder_wrapper);
};

struct texture_wrapper {
    texture_wrapper() = default;
    void write(const std::vector<uint8_t>& colors);
    void make_sampler(bool clamp_to_edge);

    bool is_valid() const;

private:
    // dawn_plugin: constructs via ptr_type constructor
    // bindgroup_wrapper: accesses m_pimpl->get_view() and get_sampler() in add_texture/add_sampler
    friend class dawn_plugin;
    friend class bindgroup_wrapper;
    DAWN_WRAPPER_PIMPL_CORE(texture_wrapper);
};

struct texture_output_wrapper {
    texture_output_wrapper() = default;
    void make_sampler(bool clamp_to_edge);

    bool is_valid() const;

private:
    // dawn_plugin: constructs via ptr_type constructor
    // bindgroup_wrapper: accesses m_pimpl->get_view() and get_sampler() in add_texture/add_sampler
    friend class dawn_plugin;
    friend class bindgroup_wrapper;
    DAWN_WRAPPER_PIMPL_CORE(texture_output_wrapper);
};

struct bindgroup_wrapper;
struct bindgroup_layout_wrapper {
    bindgroup_layout_wrapper() = default;
    bindgroup_layout_wrapper& add_buffer(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_read_only_buffer(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_uniform_buffer(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_texture_1d(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_texture_2d(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_storage_texture_2d(unsigned binding, bool enable = true);
    bindgroup_layout_wrapper& add_sampler(unsigned binding, bool enable = true);
    bindgroup_wrapper make_bindgroup();

private:
    // bindgroup_wrapper: accesses m_pimpl->make_bindGroupLayout() and constructs via ptr_type
    // compute_wrapper: accesses m_pimpl->make_bindGroupLayout() and constructs via ptr_type
    // render_wrapper: accesses m_pimpl->make_bindGroupLayout() and constructs via ptr_type
    friend class bindgroup_wrapper;
    friend class compute_wrapper;
    friend class render_wrapper;
    DAWN_WRAPPER_PIMPL_CORE(bindgroup_layout_wrapper);
};

struct bindgroup_wrapper {
    bindgroup_wrapper() = default;
    bindgroup_wrapper(bindgroup_layout_wrapper);
    bindgroup_wrapper& add_buffer(unsigned binding, buffer_wrapper);
    bindgroup_wrapper& add_texture(unsigned binding, texture_wrapper);
    bindgroup_wrapper& add_texture(unsigned binding, texture_output_wrapper);
    bindgroup_wrapper& add_sampler(unsigned binding, texture_wrapper);
    bindgroup_wrapper& add_sampler(unsigned binding, texture_output_wrapper);

    bool is_valid() const;

private:
    // bindgroup_layout_wrapper: constructs via ptr_type in make_bindgroup()
    // compute_wrapper: accesses m_pimpl->make_bindgroup() in compute()
    // render_wrapper: accesses m_pimpl->make_bindgroup() and m_pimpl in render()
    friend class bindgroup_layout_wrapper;
    friend class compute_wrapper;
    friend class render_wrapper;
    DAWN_WRAPPER_PIMPL_CORE(bindgroup_wrapper);
};

struct bindgroup_set {
    bindgroup_set();
    bindgroup_set& add_bindgroup(bindgroup_wrapper bg, unsigned group);

private:
    // compute_wrapper: accesses m_pimpl->m_bindgroups in compute()
    // render_wrapper: accesses m_pimpl->m_bindgroups in render()
    friend class compute_wrapper;
    friend class render_wrapper;
    DAWN_WRAPPER_PIMPL_CORE(bindgroup_set);
};

struct compute_wrapper {
    compute_wrapper() = default;
    void init_pipeline(bindgroup_layout_wrapper layout);
    void compile_shader(std::string script, std::string entryPoint);
    void compute(bindgroup_wrapper, unsigned width, unsigned height, encoder_wrapper encoder);
    void compute(bindgroup_set, unsigned width, unsigned height, encoder_wrapper encoder);
    void setup_compute(unsigned width, unsigned height);
    bindgroup_layout_wrapper make_bindgroup_layout();
    bool is_valid() const;

private:
    // dawn_plugin: constructs via ptr_type constructor
    friend class dawn_plugin;
    DAWN_WRAPPER_PIMPL_CORE(compute_wrapper);
};

struct surface_wrapper {
    surface_wrapper() = default;
    void setup(GLFWwindow*, unsigned width, unsigned height, bool opaque);
    void setup(std::string html_canvas_selector, unsigned width, unsigned height);
    void present();
    std::pair<unsigned, unsigned> get_width_and_height() const;

    bool is_valid() const;

private:
    // dawn_plugin: constructs via ptr_type constructor
    // render_wrapper: accesses m_pimpl->getCurrentTextureView() in render()
    friend class dawn_plugin;
    friend class render_wrapper;
    DAWN_WRAPPER_PIMPL_CORE(surface_wrapper);
};

struct render_wrapper {
    render_wrapper() = default;
    void compile_shader(std::string script, std::string entryPoint);
    void set_surface(surface_wrapper);

    void render(bindgroup_set, encoder_wrapper);
    void render(bindgroup_wrapper, encoder_wrapper);
    void render(encoder_wrapper);
    bindgroup_layout_wrapper make_bindgroup_layout();
    void init_pipeline(bindgroup_layout_wrapper);
    void init_pipeline();
    bool is_valid() const;

private:
    // dawn_plugin: constructs via ptr_type constructor
    friend class dawn_plugin;
    DAWN_WRAPPER_PIMPL_CORE(render_wrapper);
};
#undef DAWN_WRAPPER_PIMPL_CORE

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
