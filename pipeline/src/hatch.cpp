#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <asserter/src/asserter.hpp>
#include <dawn_wrapper/src/dawn_wrapper.hpp>

#include <emscripten.h>
#include <emscripten/val.h>

#include "from_js.hpp"
#include "loader.hpp"
#include "plan.hpp"

// The escape hatch, both ways, against examples/multiply.json (via pre.js). The
// stop condition it answers: "If reaching for the raw handle means everything
// above it stops working — no bookkeeping, no reuse, no diagnostics — then the
// central claim is false." So every step checks that the loader's own work
// carried on around the host's.
//
//   1. adopt: the host's 12-element buffer stands in for `source`, whose document
//      says 8 elements of init. `dest` must follow to 12, the loader must not
//      write init over the host's values, and execute()'s readback must work.
//   2. the host rewrites its buffer. The loader never hears of it.
//   3. export: the host records the loader's passes into its own encoder, copies
//      the loader's `dest` out itself, and must see step 2's values multiplied.
//   4. adopting something the document does not have as storage is an error, and
//      so is a buffer without the usages its role needs (adoption_errors).
//
// Exit 0 when every check held, 1 otherwise. Lines start with [hatch].

using emscripten::val;

namespace {

int g_failures = 0;
pipeline::instance* g_instance = nullptr; // leaked, as in main.cpp

void check(bool ok, const std::string& what)
{
    std::cout << "[hatch] " << (ok ? "ok: " : "FAIL: ") << what << std::endl;
    g_failures += ok ? 0 : 1;
}

void finish()
{
    std::cout << "[hatch] " << (g_failures ? "FAILED" : "all held") << std::endl;
    emscripten_force_exit(g_failures ? 1 : 0);
}

std::vector<float> iota_times(unsigned n, float k)
{
    std::vector<float> v(n);
    for (unsigned i = 0; i < n; ++i) {
        v[i] = float(i + 1) * k;
    }
    return v;
}

// What multiply.wgsl makes of `source`, with multiplier 4.
std::vector<double> expected(const std::vector<float>& source)
{
    std::vector<double> v(source.size(), 0);
    for (size_t i = 0; i < source.size(); i += 2) {
        v[i] = source[i] * 4.0;
    }
    return v;
}

std::string format(const std::vector<double>& v)
{
    std::stringstream s;
    s << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        s << (i ? ", " : "") << v[i];
    }
    return s.str() + "]";
}

std::vector<double> floats(size_t size, const void* data)
{
    std::vector<double> v;
    const auto* f = static_cast<const float*>(data);
    for (size_t i = 0; data && i < size / 4; ++i) {
        v.push_back(f[i]);
    }
    return v;
}

}

int main()
{
    const auto error = val::module_property("pipelineError");
    if (error.isString()) {
        std::cout << "[hatch] error: " << error.as<std::string>() << std::endl;
        emscripten_force_exit(1);
        return 1;
    }
    const auto doc = pipeline::from_js(val::module_property("pipelineDocument"));

    // 4 first: it needs no device.
    check(!pipeline::make_plan(doc, { { "params", 4 } }).ok(), "adopting a uniform is refused");
    check(!pipeline::make_plan(doc, { { "nope", 4 } }).ok(), "adopting an undeclared resource is refused");

    auto& plugin = *new dawn_wrapper::dawn_plugin;
    plugin.on_load([&plugin, doc](std::string error) {
        using namespace dawn_wrapper;

        if (!error.empty()) {
            std::cout << "[hatch] error: no device: " << error << std::endl;
            emscripten_force_exit(2);
            return;
        }

        // 1. adopt
        const auto first = iota_times(12, 1);
        auto source = plugin.make_dst_buffer(first.size() * 4, buffer_type::copy).write(first.data());
        const pipeline::adopted_buffers adopted { { "source", source } };

        const auto plan = pipeline::make_plan(doc, pipeline::adopted_lengths(adopted));
        for (const auto& e : plan.errors) {
            std::cout << "[hatch] error: " << e << std::endl;
        }
        if (!plan.ok()) {
            check(false, "the plan with `source` adopted");
            finish();
            return;
        }
        check(pipeline::adoption_errors(plan, adopted).empty(), "the host's storage buffer passes the usage check");
        {
            const auto uniform_only = plugin.make_dst_buffer(48, buffer_type::uniform);
            check(pipeline::adoption_errors(plan, { { "source", uniform_only } }).size() == 1,
                "a buffer without Storage usage is refused");
            const auto no_copy_src = plugin.make_dst_buffer(48, buffer_type::storage);
            check(pipeline::adoption_errors(plan, { { "dest", no_copy_src } }).size() == 1,
                "an output without CopySrc is refused when execute() reads back");
            check(pipeline::adoption_errors(plan, { { "dest", no_copy_src } }, false).empty(),
                "... and accepted when the host does its own copying");
        }
        check(plan.lengths.at("dest") == 12, "dest's length follows the adopted source: " + std::to_string(plan.lengths.at("dest")));

        g_instance = new pipeline::instance(plugin, doc, plan,
            [](std::string pass, std::string messages) {
                check(messages.empty(), "pass '" + pass + "' compiled" + (messages.empty() ? "" : ":\n" + messages));
            },
            adopted);

        // No check that buffer("source") is the host's by comparing sizes: the
        // loader's own would be the same size. The values below prove it instead,
        // since only the host's buffer holds them.

        g_instance->execute([&plugin, source](pipeline::values result, std::vector<std::string> errors) mutable {
            for (const auto& e : errors) {
                std::cout << "[hatch] error: " << e << std::endl;
            }
            check(errors.empty(), "execute() raised no WebGPU errors");
            check(result["dest"] == expected(iota_times(12, 1)),
                "execute() reads back dest over the host's values, not the document's init: " + format(result["dest"]));

            // 2. the host rewrites what it owns
            const auto second = iota_times(12, 10);
            source.write(second.data());

            // 3. export
            auto encoder = plugin.make_encoder();
            g_instance->encode(encoder);
            auto dest = g_instance->buffer("dest");
            auto mapped = plugin.make_dst_buffer(dest.get_size(), buffer_type::map_read);
            encoder.copy_buffer_to_buffer(dest, mapped).submit_command_buffer();
            mapped.get_output([second](size_t size, const void* data) {
                const auto got = floats(size, data);
                check(got == expected(second),
                    "the host's own encoder and copy see the host's new values: " + format(got));
                finish();
            });
        });
    });

    return 0;
}
