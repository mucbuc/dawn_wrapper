# dawn_wrapper

![Native Tests](https://github.com/mucbuc/dawn_wrapper/workflows/Native%20WebGPU%20Tests/badge.svg)
![Emscripten Tests](https://github.com/mucbuc/dawn_wrapper/workflows/Emscripten%20WebGPU%20Tests/badge.svg)

## example
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

## build
**CMakeLists.txt**
```
cmake_minimum_required(VERSION 3.27)

project(Example)

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
emcmake cmake -B web-build -DDAWN_EMSCRIPTEN_TOOLCHAIN="~/outside_of_dawn_wrapper/emscripten"
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
```
