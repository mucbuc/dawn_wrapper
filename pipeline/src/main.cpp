#include <cstdio>
#include <iostream>
#include <sstream>

#include <asserter/src/asserter.hpp>
#include <dawn_wrapper/src/dawn_wrapper.hpp>

#include <emscripten.h>
#include <emscripten/val.h>

#include "from_js.hpp"
#include "loader.hpp"
#include "plan.hpp"

// Loads the document pre.js fetched, runs it once, prints what every pass wrote
// and exits: 0 clean, 1 for a document, fetch or compile error, 2 if the GPU was
// never reached. Output lines start with [pipeline] so a script can pick them out.

using emscripten::val;

namespace {

void finish(int code)
{
    std::cout.flush();
    emscripten_force_exit(code);
}

std::string format(const std::vector<double>& v)
{
    std::stringstream s;
    s << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        s << (i ? ", " : "") << v[i];
    }
    s << "]";
    return s.str();
}

// Kept alive past main(), since the readback arrives on a later tick, and leaked
// on purpose: with a destructor at static scope, emscripten_force_exit runs it
// inside exitRuntime and aborts with "Re-entrant call to exitRuntime()".
pipeline::instance* g_instance = nullptr;

}

int main()
{
    const auto error = val::module_property("pipelineError");
    if (error.isString()) {
        std::cout << "[pipeline] error: " << error.as<std::string>() << std::endl;
        finish(1);
        return 1;
    }

    const auto doc = pipeline::from_js(val::module_property("pipelineDocument"));
    const auto plan = pipeline::make_plan(doc);
    if (!plan.ok()) {
        for (const auto& e : plan.errors) {
            std::cout << "[pipeline] error: " << e << std::endl;
        }
        finish(1);
        return 1;
    }

    for (const auto& p : plan.passes) {
        std::cout << "[pipeline] pass '" << p.name << "', " << p.workgroups << " workgroup(s):\n"
                  << p.wgsl << std::endl;
    }

    // Leaked for the same reason as g_instance.
    auto& plugin = *new dawn_wrapper::dawn_plugin;
    plugin.on_load([&plugin, doc, plan](std::string error) {
        if (!error.empty()) {
            std::cout << "[pipeline] error: no device: " << error << std::endl;
            finish(2);
            return;
        }

        auto clean = std::make_shared<bool>(true);
        g_instance = new pipeline::instance(plugin, doc, plan,
            [clean](std::string pass, std::string messages) {
                if (!messages.empty()) {
                    *clean = false;
                    std::cout << "[pipeline] error: pass '" << pass << "' did not compile:\n"
                              << messages << std::endl;
                }
            });

        g_instance->execute([clean](pipeline::values result, std::vector<std::string> errors) {
            for (const auto& e : errors) {
                std::cout << "[pipeline] error: " << e << std::endl;
            }
            // Nothing is printed as a result unless it is one: a pass that did not
            // compile, or any error in the scopes, leaves `result` meaningless.
            const bool ok = *clean && errors.empty();
            for (const auto& [name, v] : ok ? result : pipeline::values {}) {
                std::cout << "[pipeline] " << name << " = " << format(v) << std::endl;
            }
            finish(ok ? 0 : 1);
        });
    });

    return 0;
}
