// What render_config actually does, and what a failed compile reports.
//
// Both are new and both were unexercised: driftype only ever uses the defaults,
// so the byte-identical frames it produces prove nothing about a supplied vertex
// shader, instanced drawing with no vertex buffer, additive blending or another
// topology. This covers pipeline CREATION for those, which is where a malformed
// descriptor shows up. It does not draw: a native run has no surface.
//
// Built by the test harness beside example.cpp.

#include <functional>
#include <iostream>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <asserter/src/asserter.hpp>
#include <dawn_wrapper/src/dawn_wrapper.hpp>

using namespace dawn_wrapper;

namespace {

// Positions built from the vertex index, nothing bound: the shape a particle
// system uses, and the case the wrapper could not express before.
const char* vertex_source = R"(
struct Out {
    @builtin(position) pos: vec4f,
    @location(0) tint: f32,
};

@vertex fn vs(@builtin(vertex_index) v: u32, @builtin(instance_index) i: u32) -> Out {
    var corners = array<vec2f, 3>(vec2f(-1.0, -1.0), vec2f(3.0, -1.0), vec2f(-1.0, 3.0));
    var out: Out;
    out.pos = vec4f(corners[v], 0.0, 1.0);
    out.tint = f32(i) / 64.0;
    return out;
}
)";

const char* fragment_source = R"(
@fragment fn fs(@location(0) tint: f32) -> @location(0) vec4f {
    return vec4f(tint, 0.0, 0.0, 1.0);
}
)";

// The built-in vertex stage takes one Float32x2 at location 0; this is the shape
// driftype compiles, and it must keep working.
const char* default_fragment_source = R"(
@fragment fn fragmentMain() -> @location(0) vec4f {
    return vec4f(0.0, 1.0, 0.0, 1.0);
}
)";

const char* broken_fragment_source = R"(
@fragment fn fs(@location(0) tint: f32) -> @location(0) vec4f {
    return this_is_not_wgsl;
}
)";

using done_callback = std::function<void()>;

// GetCompilationInfo answers late. Natively the caller has to keep ticking the
// device until it does; bounded, so a callback that never arrives fails the test
// rather than hanging it. In the browser the answer is a promise, and it cannot
// resolve while this code holds the thread, so there the test continues from the
// callback instead of waiting for it (as example.cpp does with its buffer map).
void compile_then(dawn_plugin plugin, render_wrapper render,
    const char* source, const char* entry, std::function<void(std::string)> next)
{
#ifdef __EMSCRIPTEN__
    (void)plugin;
    render.compile_shader(source, entry, std::move(next));
#else
    bool answered = false;
    std::string messages;
    render.compile_shader(source, entry, [&](std::string text) {
        messages = std::move(text);
        answered = true;
    });

    for (int i = 0; i < 1000 && !answered; ++i) {
        plugin.run();
    }
    ASSERT(answered);
    next(std::move(messages));
#endif
}

void finish(int code)
{
#ifdef __EMSCRIPTEN__
    emscripten_force_exit(code);
#else
    (void)code;
#endif
}

void a_configured_pipeline_builds(dawn_plugin plugin, done_callback done)
{
    auto render = plugin.make_render();

    render_config config;
    config.vertex_script = vertex_source;
    config.vertex_entry = "vs";
    config.blend = blend_mode::additive;
    config.primitive = topology::point_list;
    config.target = texture_format::rgba8unorm;
    config.vertex_buffer = false;
    config.vertex_count = 3;
    config.instance_count = 64;
    render.configure(config);

    compile_then(plugin, render, fragment_source, "fs", [render, done](std::string messages) mutable {
        ASSERT(messages.empty());

        render.init_pipeline();
        ASSERT(render.is_valid());
        std::cout << "  ok: supplied vertex stage, no vertex buffer, additive, points, rgba8unorm"
                  << std::endl;
        done();
    });
}

// Both stages from compile_shader's one module (vertex_from_module), a render
// that only records (submit = false), and the pieces a textured pass binds: a
// sampler of its own, a texture written from the host, and bindings seen by one
// stage only (set_visibility).
void one_module_pipeline_builds(dawn_plugin plugin, done_callback done)
{
    auto render = plugin.make_render();

    render_config config;
    config.vertex_from_module = true;
    config.vertex_entry = "vs";
    config.target = texture_format::rgba8unorm;
    config.vertex_buffer = false;
    config.submit = false;
    render.configure(config);

    auto sampler = plugin.make_sampler({ filter_mode::nearest, address_mode::mirror_repeat });
    ASSERT(sampler.is_valid());
    auto texture = plugin.make_texture_output(2, 2);
    texture.write(std::vector<uint8_t>(2 * 2 * 4, 255));
    ASSERT(plugin.make_texture_2d(3, 5).get_width() == 3 && plugin.make_texture_2d(3, 5).get_height() == 5);

    const std::string source = std::string(vertex_source) + fragment_source;
    compile_then(plugin, render, source.c_str(), "fs", [plugin, render, sampler, texture, done](std::string messages) mutable {
        ASSERT(messages.empty());

        auto layout = render.make_bindgroup_layout(shader_visibility::vertex_and_fragment)
                          .add_sampler(0)
                          .add_texture_2d(1)
                          .set_visibility(0, shader_visibility::fragment)
                          .set_visibility(1, shader_visibility::fragment);
        layout.make_bindgroup().add_sampler(0, sampler).add_texture(1, texture);
        render.init_pipeline(layout);
        ASSERT(render.is_valid());
        std::cout << "  ok: one module for both stages, record-only, a nearest sampler, per-binding visibility"
                  << std::endl;
        done();
    });
}

void the_default_pipeline_still_builds(dawn_plugin plugin, done_callback done)
{
    auto render = plugin.make_render();

    compile_then(plugin, render, default_fragment_source, "fragmentMain",
        [render, done](std::string messages) mutable {
            ASSERT(messages.empty());

            render.init_pipeline();
            ASSERT(render.is_valid());
            std::cout << "  ok: defaults unchanged — built-in vertex stage, indexed triangle"
                      << std::endl;
            done();
        });
}

// Opt-in: ./RenderConfig --broken
//
// Natively this ABORTS, by design and not by accident. A failed
// CreateShaderModule raises an uncaptured device error, and dawn_wrapper's
// uncaptured handler asserts on the native path while the Emscripten path reports
// to the page instead (see SetUncapturedErrorCallback in dawn_wrapper.cpp). So the
// interesting half of this — that the compile callback receives the compiler's own
// text rather than a bare "a shader failed" — cannot be asserted in-process here.
// It is verified in the browser instead: driftype renders the fallback and logs
// "[driftype] shader compile: Error(0): ..." with the offending line.
//
// Run it to watch what the native path does; expect the assert.
void a_broken_shader_reports_itself(dawn_plugin plugin, done_callback done)
{
    auto render = plugin.make_render();

    compile_then(plugin, render, broken_fragment_source, "fs", [done](std::string messages) {
        ASSERT(!messages.empty());
        ASSERT(messages.find("Error(") != std::string::npos);
        // The message is the compiler's, not a restatement: it has to name something
        // from the source rather than only saying that a shader failed.
        ASSERT(messages.find("this_is_not_wgsl") != std::string::npos);
        std::cout << "  ok: a broken shader reaches the callback, with the compiler's text"
                  << std::endl;
        done();
    });
}

} // namespace

int main(int argc, char** argv)
{
    const bool broken = argc > 1 && std::string(argv[1]) == "--broken";

    dawn_plugin plugin;

    plugin.on_load([plugin, broken](std::string error) {
        ASSERT(error.empty());

        auto instance = plugin;
        if (broken) {
            a_broken_shader_reports_itself(instance, [] { finish(0); });
            return;
        }

        a_configured_pipeline_builds(instance, [instance] {
            one_module_pipeline_builds(instance, [instance] {
                the_default_pipeline_still_builds(instance, [] {
                    std::cout << "render_config: all checks passed" << std::endl;
                    finish(0);
                });
            });
        });
    });

    return 0;
}
