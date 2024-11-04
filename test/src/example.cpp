#include <iostream>

#include <lib/dawn_wrapper/src/dawn_wrapper.h>

using namespace std;
using namespace dawn_wrapper;

int main()
{
    enum {
        binding_in = 1,
        binding_out = 2,
        workgroup_size = 8,
    };
    const auto entry_point = "go";

    dawn_plugin plugin;

    // compile shader
    auto comp = plugin.make_compute();
    comp.compile_shader(R"(
        @group(0) @binding({{binding_in}}) var<storage, read> inputBuffer : array<u32>;
        @group(0) @binding({{binding_out}}) var<storage, read_write> outputBuffer: array<u32>;
        @compute @workgroup_size({{workgroup_size}})
        fn {{entry_point}}(@builtin(global_invocation_id) id: vec3<u32>) {
            outputBuffer[id.x] = inputBuffer[id.x] + 2;
        })",
        entry_point,
        {
            { "workgroup_size", to_string(workgroup_size) },
            { "binding_in", to_string(binding_in) },
            { "binding_out", to_string(binding_out) },
            { "entry_point", entry_point },
        });

    // layout and pipeline
    auto layout = comp.make_bindgroup_layout().add_read_only_buffer(binding_in).add_buffer(binding_out);
    comp.init_pipeline(layout);

    // io buffers
    using vector_type = vector<uint32_t>;
    const vector_type data = { 0, 1, 3, 5, 7, 11, 13, 17, 19 };
    const auto size_bytes = data.size() * sizeof(vector_type::value_type);
    buffer_wrapper input = plugin.make_buffer(size_bytes, BufferType::Storage, true)
                               .write(data.data());
    buffer_wrapper output = plugin.make_buffer(size_bytes, BufferType::Storage, false);

    // compute
    auto bindgroup = comp.make_bindgroup().add_buffer(binding_in, input).add_buffer(binding_out, output);
    auto encoder = plugin.make_encoder();
    comp.compute(bindgroup, unsigned(data.size()), 1, encoder);
    plugin.run();

    // mapped buffer
    buffer_wrapper mapped = plugin.make_buffer(size_bytes, BufferType::MapRead, true)
                                .write(data.data());

    encoder.copy_buffer_to_buffer(output, mapped).submit_command_buffer();
    mapped.get_output([](auto size, auto data) {
        const uint32_t* p = reinterpret_cast<const uint32_t*>(data);

        for (auto i = 0; i < size / sizeof(uint32_t); ++i) {
            cout << p[i] << endl;
        }
    });

    while (!mapped.done()) {
        plugin.run();
    }

    return 0;
}
