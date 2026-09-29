#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <asserter/src/asserter.hpp>
#include <dawn_wrapper/src/dawn_wrapper.hpp>

#include <emscripten.h>

// Step 1 of the plan for the particle target: written by hand on dawn_wrapper,
// no loader, to prove the three wrapper gaps are closed before anything is
// generated against them:
//
//   1. a vertex stage reads a storage buffer: render_wrapper's layouts can now be
//      visible to the vertex stage, and without that the pipeline is invalid
//      (checked first, against the old default);
//   2. it draws offscreen: render_wrapper::set_target, no canvas;
//   3. the frame comes back: encoder_wrapper::copy_texture_to_buffer.
//
// Four particles, drawn as discs of radius 4px into 60x64, additive. A compute
// pass moves the first by its velocity before the draw; the third and fourth sit
// on the same spot, so additive blending doubles them. Every check is against a
// number worked out here on the CPU, pixel for pixel. Exit 0 when all held.
//
// Lines start with [particles].

namespace {

using namespace dawn_wrapper;

constexpr unsigned W = 60, H = 64; // 60: rows of 240 bytes, so the 256 padding is exercised
constexpr float R = 4;

struct particle {
    float x, y, vx, vy;
};

// Pixels, y down. The first moves 8px right in the one step the compute takes.
const std::vector<particle> start {
    { 16, 16, 8, 0 },
    { 48, 16, 0, 0 },
    { 16, 48, 0, 0 },
    { 16, 48, 0, 0 },
};

const char* compute_wgsl = R"(
struct Particle { pos: vec2f, vel: vec2f }
struct Sim { dt: f32 }
@group(0) @binding(0) var<storage, read_write> particles: array<Particle>;
@group(1) @binding(0) var<uniform> sim: Sim;

@compute @workgroup_size(64)
fn step(@builtin(global_invocation_id) gid: vec3u) {
    let i = gid.x;
    if (i >= arrayLength(&particles)) { return; }
    particles[i].pos += particles[i].vel * sim.dt;
}
)";

// Read-only here: a vertex stage cannot bind writable storage.
const char* draw_wgsl = R"(
struct Particle { pos: vec2f, vel: vec2f }
struct View { size: vec2f, radius: f32 }
@group(0) @binding(0) var<storage, read> particles: array<Particle>;
@group(1) @binding(0) var<uniform> view: View;

struct Out {
    @builtin(position) position: vec4f,
    @location(0) centre: vec2f,
}

@vertex
fn vs(@builtin(vertex_index) v: u32, @builtin(instance_index) i: u32) -> Out {
    var corners = array<vec2f, 6>(
        vec2f(-1, -1), vec2f(1, -1), vec2f(1, 1),
        vec2f(-1, -1), vec2f(1, 1), vec2f(-1, 1));
    let centre = particles[i].pos;
    let p = centre + corners[v] * view.radius;
    var out: Out;
    out.position = vec4f(p.x / view.size.x * 2 - 1, 1 - p.y / view.size.y * 2, 0, 1);
    out.centre = centre;
    return out;
}

@fragment
fn fs(in: Out) -> @location(0) vec4f {
    if (distance(in.position.xy, in.centre) > view.radius) { discard; }
    return vec4f(0.25);
}
)";

int g_failures = 0;

void check(bool ok, const std::string& what)
{
    std::cout << "[particles] " << (ok ? "ok: " : "FAIL: ") << what << std::endl;
    g_failures += ok ? 0 : 1;
}

void finish()
{
    std::cout << "[particles] " << (g_failures ? "FAILED" : "all held") << std::endl;
    emscripten_force_exit(g_failures ? 1 : 0);
}

// How many discs cover pixel (x, y), sampled at its centre as the rasteriser
// does. Centres sit on whole pixels, so a pixel centre is never exactly R away:
// no ties to disagree over.
unsigned coverage(const std::vector<particle>& ps, unsigned x, unsigned y)
{
    unsigned n = 0;
    for (const auto& p : ps) {
        const float dx = x + 0.5f - p.x, dy = y + 0.5f - p.y;
        n += dx * dx + dy * dy <= R * R ? 1 : 0;
    }
    return n;
}

render_config draw_config()
{
    render_config config;
    config.vertex_script = draw_wgsl;
    config.vertex_entry = "vs";
    config.target = texture_format::rgba8unorm;
    config.blend = blend_mode::additive;
    config.primitive = topology::triangle_list;
    config.vertex_buffer = false;
    config.vertex_count = 6;
    config.instance_count = unsigned(start.size());
    return config;
}

std::string compiled(const std::string& what, std::string messages)
{
    check(messages.empty(), what + " compiled" + (messages.empty() ? "" : ":\n" + messages));
    return messages;
}

}

int main()
{
    auto& plugin = *new dawn_plugin; // leaked, as in main.cpp
    plugin.on_load([&plugin](std::string error) {
        if (!error.empty()) {
            std::cout << "[particles] error: no device: " << error << std::endl;
            emscripten_force_exit(2);
            return;
        }

        // 1, the gap as it was: the default layout is fragment-only, and a vertex
        // stage reading `particles` through it must be refused.
        plugin.push_error_scope();
        {
            auto render = plugin.make_render();
            render.configure(draw_config());
            render.compile_shader(draw_wgsl, "fs");
            auto particles = render.make_bindgroup_layout().add_read_only_buffer(0);
            auto view = render.make_bindgroup_layout(shader_visibility::vertex_and_fragment).add_uniform_buffer(0);
            render.init_pipeline({ particles, view });
        }
        plugin.pop_error_scope([](std::string error) {
            check(!error.empty(), "a fragment-only layout for the vertex stage's storage is refused" + (error.empty() ? std::string() : ": " + error.substr(0, 90) + "..."));
        });

        plugin.push_error_scope();

        std::vector<float> packed;
        for (const auto& p : start) {
            packed.insert(packed.end(), { p.x, p.y, p.vx, p.vy });
        }
        auto particles = plugin.make_dst_buffer(packed.size() * 4, buffer_type::copy).write(packed.data());
        const float sim_values[4] = { 1, 0, 0, 0 };
        auto sim = plugin.make_dst_buffer(16, buffer_type::uniform).write(sim_values);
        const float view_values[4] = { W, H, R, 0 };
        auto view = plugin.make_dst_buffer(16, buffer_type::uniform).write(view_values);

        auto compute = plugin.make_compute();
        compute.compile_shader(compute_wgsl, "step", [](std::string m) { compiled("the compute pass", m); });
        auto c_particles = compute.make_bindgroup_layout().add_buffer(0);
        auto c_sim = compute.make_bindgroup_layout().add_uniform_buffer(0);
        compute.init_pipeline({ c_particles, c_sim });
        bindgroup_set c_set;
        c_set.add_bindgroup(c_particles.make_bindgroup().add_buffer(0, particles), 0);
        c_set.add_bindgroup(c_sim.make_bindgroup().add_buffer(0, sim), 1);

        auto target = plugin.make_texture_output(W, H);
        auto render = plugin.make_render();
        render.configure(draw_config());
        render.set_target(target);
        render.compile_shader(draw_wgsl, "fs", [](std::string m) { compiled("the draw", m); });
        auto r_particles = render.make_bindgroup_layout(shader_visibility::vertex).add_read_only_buffer(0);
        auto r_view = render.make_bindgroup_layout(shader_visibility::vertex_and_fragment).add_uniform_buffer(0);
        render.init_pipeline({ r_particles, r_view });
        bindgroup_set r_set;
        r_set.add_bindgroup(r_particles.make_bindgroup().add_buffer(0, particles), 0);
        r_set.add_bindgroup(r_view.make_bindgroup().add_buffer(0, view), 1);

        // One frame: the compute, then the draw, in one encoder (render() submits).
        auto frame = plugin.make_encoder();
        compute.compute(c_set, 1, 1, frame);
        render.render(r_set, frame);

        auto readback = plugin.make_encoder();
        auto mapped = plugin.make_dst_buffer(target.readback_size(), buffer_type::map_read);
        readback.copy_texture_to_buffer(target, mapped).submit_command_buffer();

        plugin.pop_error_scope([mapped, target](std::string error) mutable {
            check(error.empty(), "building and drawing raised no WebGPU errors" + (error.empty() ? "" : ": " + error));

            mapped.get_output([target](size_t size, const void* data) {
                check(data && size == target.readback_size(), "the frame came back: " + std::to_string(size) + " bytes, rows of " + std::to_string(target.bytes_per_row()));
                if (!data) {
                    finish();
                    return;
                }
                const auto* px = static_cast<const uint8_t*>(data);
                const auto red = [&](unsigned x, unsigned y) { return px[y * target.bytes_per_row() + x * 4]; };

                // Where the compute pass should have left them.
                auto moved = start;
                for (auto& p : moved) {
                    p.x += p.vx;
                    p.y += p.vy;
                }

                unsigned ink = 0, expected_ink = 0, wrong = 0;
                for (unsigned y = 0; y < H; ++y) {
                    for (unsigned x = 0; x < W; ++x) {
                        const unsigned n = coverage(moved, x, y);
                        const int want = int(std::lround(std::min(1.0f, 0.25f * n) * 255));
                        ink += red(x, y) ? 1 : 0;
                        expected_ink += n ? 1 : 0;
                        wrong += std::abs(int(red(x, y)) - want) > 1 ? 1 : 0;
                    }
                }
                check(ink == expected_ink, "ink matches the discs pixel for pixel in count: " + std::to_string(ink) + " of " + std::to_string(expected_ink));
                check(wrong == 0, "every pixel within 1/255 of its expected value (" + std::to_string(wrong) + " off)");
                check(red(24, 16) > 0 && red(16, 16) == 0, "the compute pass moved particle 0 before the draw read it (lit at x=24, dark at x=16)");
                check(std::abs(int(red(16, 48)) - 128) <= 1 && std::abs(int(red(48, 16)) - 64) <= 1,
                    "additive: the doubled spot is " + std::to_string(red(16, 48)) + ", a single one " + std::to_string(red(48, 16)));
                finish();
            });
        });
    });

    return 0;
}
