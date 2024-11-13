#include <iostream>
#include <sstream>

#include <lib/dawn_wrapper/src/dawn_wrapper.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten/html5.h>
#endif

using namespace std;
using namespace dawn_wrapper;

int main()
{
    using vector_type = vector<uint32_t>;



#ifdef __EMSCRIPTEN__

    emscripten_set_main_loop_arg(
            [](void *userData) {

                const vector_type data = { 0, 1, 3, 5, 7, 11, 13, 17, 19 };
                const unsigned size_bytes = unsigned(data.size()) * sizeof(vector_type::value_type);

                enum {
                    binding_in = 1,
                    binding_out = 2,
                    workgroup_size = 8,
                };

                static dawn_plugin plugin;

                std::cout << "main loop arg" << (plugin ? "done": "not done") << std::endl;

                if (plugin) {
                    auto comp = plugin.make_compute();

                    const auto entry_point = "go";;

                    stringstream shader_script;
                    shader_script
                        << "@group(0) @binding(" << binding_in << ") var<storage, read> inputBuffer : array<u32>;\n"
                        << "@group(0) @binding(" << binding_out << ") var<storage, read_write> outputBuffer: array<u32>;\n"
                        << "@compute @workgroup_size(" << workgroup_size << ")\n"
                        << "fn " << entry_point << "(@builtin(global_invocation_id) id: vec3<u32>) {\n"
                        << "outputBuffer[id.x] = inputBuffer[id.x] + 2;"
                        << "}";
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
//                        plugin.run();

                        // get result
                        encoder.copy_buffer_to_buffer(output, mapped).submit_command_buffer();
                        mapped.get_output([](auto size, auto data) {
                            const uint32_t* p = reinterpret_cast<const vector_type::value_type*>(data);
                            for (auto i = 0; i < size / sizeof(vector_type::value_type); ++i) {
                                cout << p[i] << endl;
                            }
                        });
                        //while (!mapped.done()) {
                        //    plugin.run();
                        //}
                }
            },
            nullptr,
            0, true
        );
#else
    const vector_type data = { 0, 1, 3, 5, 7, 11, 13, 17, 19 };
    const unsigned size_bytes = unsigned(data.size()) * sizeof(vector_type::value_type);

    enum {
        binding_in = 1,
        binding_out = 2,
        workgroup_size = 8,
    };

    static dawn_plugin plugin;


    // compile shader
    auto comp = plugin.make_compute();
    const auto entry_point = "go";

    stringstream shader_script;
    shader_script
        << "@group(0) @binding(" << binding_in << ") var<storage, read> inputBuffer : array<u32>;\n"
        << "@group(0) @binding(" << binding_out << ") var<storage, read_write> outputBuffer: array<u32>;\n"
        << "@compute @workgroup_size(" << workgroup_size << ")\n"
        << "fn " << entry_point << "(@builtin(global_invocation_id) id: vec3<u32>) {\n"
        << "outputBuffer[id.x] = inputBuffer[id.x] + 2;"
        << "}";

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
#endif
    std::cout << "test" << std::endl;

    return 0;
}
