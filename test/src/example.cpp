#include <iostream>

#include <lib/dawn_wrapper/src/dawn_wrapper.h>

using namespace std;
using namespace dawn_wrapper;

int main()
{
    dawn_plugin plugin;

    // compile shader
    auto comp = plugin.make_compute();
    comp.compile_shader(R"(
        @group(0) @binding(1) var<storage, read> inputBuffer : array<u32>;
        @group(0) @binding(2) var<storage, read_write> outputBuffer: array<u32>;
        
        @compute @workgroup_size(8)
        fn computeStuff(@builtin(global_invocation_id) id: vec3<u32>) {
            
            outputBuffer[id.x] = inputBuffer[id.x] + 2;
            
        })",
        "computeStuff");

    // layout and pipeline
    auto layout = comp.make_bindgroup_layout().addReadOnlyBuffer(1).addBuffer(2);
    comp.init_pipeline(layout);

    // io buffers
    const vector<uint32_t> data = { 0, 1, 3, 5, 7, 11, 13, 17, 19 };
    buffer_wrapper input = plugin.make_buffer(BufferType::Storage, true)
                               .write(data.data(), data.size() * sizeof(uint32_t));
    buffer_wrapper output = plugin.make_buffer(data.size() * sizeof(uint32_t), BufferType::Storage, false);

    // compute
    auto bindgroup = comp.make_bindgroup().addBuffer(1, input).addBuffer(2, output);
    auto encoder = plugin.make_encoder();
    comp.compute(bindgroup, unsigned(data.size()), 1, encoder);
    plugin.run();

    // mapped buffer
    buffer_wrapper mapped = plugin.make_buffer(BufferType::MapRead, true)
                                .write(data.data(), data.size() * sizeof(uint32_t));

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
