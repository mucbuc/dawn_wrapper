#pragma once

#include <array>
#include <iostream>
#include <map>

struct GLFWwindow;
namespace dawn_wrapper {

#define WRAPPER_PIMPL_DEC(class_name)        \
private:                                     \
    friend class render_wrapper;             \
    friend class compute_wrapper;            \
    friend class dawn_plugin;                \
    friend class texture_wrapper;            \
    friend class bindgroup_wrapper;          \
    friend class buffer_wrapper;             \
    friend class bindgroup_layout_wrapper;   \
    struct pimpl;                            \
    using ptr_type = std::shared_ptr<pimpl>; \
    class_name(ptr_type);                    \
    ptr_type m_pimpl;

struct encoder_wrapper {
    encoder_wrapper() = default;
    void submit_command_buffer();
    WRAPPER_PIMPL_DEC(encoder_wrapper);
};

struct buffer_wrapper {
    buffer_wrapper() = default;
    void write(const std::vector<uint8_t>& colors);
    void write(void*, size_t);
    WRAPPER_PIMPL_DEC(buffer_wrapper);
};

struct texture_wrapper {
    texture_wrapper() = default;
    void write(const std::vector<uint8_t>& colors);
    void make_sampler(bool clamp_to_edge);
    WRAPPER_PIMPL_DEC(texture_wrapper);
};

struct bindgroup_layout_wrapper {
    bindgroup_layout_wrapper() = delete;
    void addBuffer(unsigned binding);
    void addReadOnlyBuffer(unsigned binding);
    void addUniformBuffer(unsigned binding);
    void addTexture(unsigned binding);
    void addSampler(unsigned binding);
    WRAPPER_PIMPL_DEC(bindgroup_layout_wrapper);
};

struct bindgroup_wrapper {
    bindgroup_wrapper() = delete;
    void addBuffer(unsigned binding, buffer_wrapper);
    void addTexture(unsigned binding, texture_wrapper);
    void addSampler(unsigned binding, texture_wrapper);
    WRAPPER_PIMPL_DEC(bindgroup_wrapper);
};

struct compute_wrapper {
    compute_wrapper() = default;
    void make_pipeline(bindgroup_layout_wrapper layout);
    void make_shader(std::string script, std::string entryPoint);
    bool compute(bindgroup_wrapper, unsigned width, unsigned height, encoder_wrapper encoder);
    void setup_compute(unsigned width, unsigned height);
    bindgroup_layout_wrapper make_bindgroup_layout();
    bindgroup_wrapper make_bindgroup();
    WRAPPER_PIMPL_DEC(compute_wrapper);
};

struct render_wrapper {
    render_wrapper() = default;
    void make_shader(std::string script, std::string entryPoint);
    void setup_surface(GLFWwindow*, unsigned width, unsigned height, bool opaque);
    void render(bindgroup_wrapper, encoder_wrapper);
    bindgroup_layout_wrapper make_bindgroup_layout();
    bindgroup_wrapper make_bindgroup();
    void make_pipeline(bindgroup_layout_wrapper);
    WRAPPER_PIMPL_DEC(render_wrapper);
};
#undef WRAPPER_PIMPL_DEC

enum class BufferType {
    Storage,
    Uniform,
    Index,
    Vertex,
};

struct dawn_plugin {
    dawn_plugin();
    ~dawn_plugin();
    render_wrapper make_render();
    compute_wrapper make_compute();
    buffer_wrapper make_buffer(unsigned size, BufferType type, bool isDest = true);
    buffer_wrapper make_buffer(BufferType type, bool isDest = true);
    texture_wrapper make_texture(unsigned);
    texture_wrapper make_texture(std::vector<uint8_t> data);
    encoder_wrapper make_encoder();
    bool run();

private:
    struct dawn_pimpl;
    std::shared_ptr<dawn_pimpl> m_pimpl;
};
}