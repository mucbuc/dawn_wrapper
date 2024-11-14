# install [dawn](https://dawn.googlesource.com/dawn)
```
cd lib
git clone --depth 1 https://dawn.googlesource.com/dawn
```

# example
**example.cpp**
```
#include <iostream>
#include <sstream>

#include <lib/dawn_wrapper/src/dawn_wrapper.h>

using namespace std;
using namespace dawn_wrapper;

int main()
{
    using vector_type = vector<uint32_t>;
    const vector_type data = { 0, 1, 3, 5, 7, 11, 13, 17, 19 };
    const unsigned size_bytes = unsigned(data.size()) * sizeof(vector_type::value_type);

    enum {
        binding_in = 1,
        binding_out = 2,
        workgroup_size = 8,
    };
    const auto entry_point = "go";

    stringstream shader_script;
    shader_script
        << "@group(0) @binding(" << binding_in << ") var<storage, read> inputBuffer : array<u32>;\n"
        << "@group(0) @binding(" << binding_out << ") var<storage, read_write> outputBuffer: array<u32>;\n"
        << "@compute @workgroup_size(" << workgroup_size << ")\n"
        << "fn " << entry_point << "(@builtin(global_invocation_id) id: vec3<u32>) {\n"
        << "outputBuffer[id.x] = inputBuffer[id.x] + 2;"
        << "}";

    dawn_plugin plugin;

    // compile shader
    auto comp = plugin.make_compute();
    comp.compile_shader( shader_script.str(), entry_point );

    // layout and pipeline
    auto layout = comp.make_bindgroup_layout().add_read_only_buffer(binding_in).add_buffer(binding_out);
    comp.init_pipeline(layout);

    // buffers
    buffer_wrapper input = plugin.make_dst_buffer(size_bytes, buffer_type::storage).write(data.data());
    buffer_wrapper output = plugin.make_src_buffer(size_bytes, buffer_type::storage);
    buffer_wrapper mapped = plugin.make_dst_buffer(size_bytes, buffer_type::map_read);

    // compute
    auto bindgroup = comp.make_bindgroup().add_buffer(binding_in, input).add_buffer(binding_out, output);
    auto encoder = plugin.make_encoder();
    comp.compute(bindgroup, unsigned(data.size()), 1, encoder);
    plugin.run();

    // get result
    encoder.copy_buffer_to_buffer(output, mapped).submit_command_buffer();
    mapped.get_output([](auto size, auto data) {
        const uint32_t* p = reinterpret_cast<const vector_type::value_type*>(data);
        for (auto i = 0; i < size / sizeof(vector_type::value_type); ++i) {
            cout << p[i] << endl;
        }
    });

    while (!mapped.done()) {
        plugin.run();
    }

    return 0;
}
```
**CMakeLists.txt**
```
project(Example)
cmake_minimum_required(VERSION 3.27)

add_executable(Example src/example.cpp)
set_target_properties(Example PROPERTIES
    CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON
)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
add_compile_definitions(TARGET_HEADLESS)
add_subdirectory(lib/dawn_wrapper)

if (EMSCRIPTEN)
    set_target_properties(Example PROPERTIES SUFFIX ".html")
    target_link_options(Example PRIVATE "-sUSE_WEBGPU=1" "-sUSE_GLFW=3")
endif()

target_link_libraries(Example PRIVATE
    dawn_wrapper
)

target_include_directories(Example SYSTEM PUBLIC ${CMAKE_SOURCE_DIR})
```
**build native:**
```
cmake -B build
cmake --build build -j 8
```
**build emscripten:**
```
emcmake cmake -B web-build -DDAWN_EMSCRIPTEN_TOOLCHAIN="/Users/mucbuc/work/emscripten"
cmake --build web-build
```
**output:**
```
2
3
5
7
9
13
15
19
21
device lost: A valid external Instance reference no longer exists.
```

# interface
```
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
    unsigned get_size();

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
```
