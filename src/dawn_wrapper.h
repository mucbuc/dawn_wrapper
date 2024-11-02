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
    friend class texture_output_wrapper;     \
    friend class bindgroup_wrapper;          \
    friend class buffer_wrapper;             \
    friend class bindgroup_layout_wrapper;   \
    friend class encoder_wrapper;            \
    struct pimpl;                            \
    using ptr_type = std::shared_ptr<pimpl>; \
    class_name(ptr_type);                    \
    ptr_type m_pimpl;

struct buffer_wrapper {
    buffer_wrapper() = default;
    buffer_wrapper & write(const std::vector<uint8_t>& colors);
    buffer_wrapper & write(void*, size_t);
    bool done();
    buffer_wrapper & get_output(std::function<void(unsigned, const void*)>);
    size_t get_size();

    operator bool() const;
    WRAPPER_PIMPL_DEC(buffer_wrapper);
};

struct encoder_wrapper {
    encoder_wrapper() = default;
    encoder_wrapper & submit_command_buffer();
    encoder_wrapper & copy_buffer_to_buffer(buffer_wrapper, buffer_wrapper, size_t offset = 0);
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
    bindgroup_layout_wrapper & addBuffer(unsigned binding);
    bindgroup_layout_wrapper & addReadOnlyBuffer(unsigned binding);
    bindgroup_layout_wrapper & addUniformBuffer(unsigned binding);
    bindgroup_layout_wrapper & addTexture_1d(unsigned binding);
    bindgroup_layout_wrapper & addTexture_2d(unsigned binding);
    bindgroup_layout_wrapper & addStorageTexture_2d(unsigned binding);
    bindgroup_layout_wrapper & addSampler(unsigned binding);
    WRAPPER_PIMPL_DEC(bindgroup_layout_wrapper);
};

struct bindgroup_wrapper {
    bindgroup_wrapper() = default;
    bindgroup_wrapper & addBuffer(unsigned binding, buffer_wrapper);
    bindgroup_wrapper & addTexture(unsigned binding, texture_wrapper);
    bindgroup_wrapper & addTexture(unsigned binding, texture_output_wrapper);
    bindgroup_wrapper & addSampler(unsigned binding, texture_wrapper);
    bindgroup_wrapper & addSampler(unsigned binding, texture_output_wrapper);

    operator bool() const;
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
    operator bool() const;
    WRAPPER_PIMPL_DEC(compute_wrapper);
};

struct render_wrapper {
    render_wrapper() = default;
    void make_shader(std::string script, std::string entryPoint);
    void make_shader(std::string script, std::string entryPoint, std::map<std::string, std::string> variables);
    void setup_surface(GLFWwindow*, unsigned width, unsigned height, bool opaque);
    void render(bindgroup_wrapper, encoder_wrapper);
    void render(encoder_wrapper);
    bindgroup_layout_wrapper make_bindgroup_layout();
    bindgroup_wrapper make_bindgroup();
    void make_pipeline(bindgroup_layout_wrapper);
    void make_pipeline();
    operator bool() const;
    WRAPPER_PIMPL_DEC(render_wrapper);
};
#undef WRAPPER_PIMPL_DEC

enum class BufferType {
    Storage,
    Uniform,
    Index,
    Vertex,
    MapRead,
};

struct dawn_plugin {
    dawn_plugin();
    ~dawn_plugin();
    render_wrapper make_render();
    compute_wrapper make_compute();
    buffer_wrapper make_buffer(size_t size, BufferType type, bool isDest = true);
    buffer_wrapper make_buffer(BufferType type, bool isDest = true);
    texture_wrapper make_texture(size_t);
    texture_wrapper make_texture(size_t, size_t);
    texture_wrapper make_texture(std::vector<uint8_t> data);
    texture_output_wrapper make_texture_output(size_t, size_t);
    encoder_wrapper make_encoder();
    bool run();

private:
    struct dawn_pimpl;
    std::shared_ptr<dawn_pimpl> m_pimpl;
};
}
