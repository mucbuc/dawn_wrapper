#pragma once

#include <algorithm>
#include <cstring>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <asserter/src/asserter.hpp>
#include <dawn_wrapper/src/dawn_wrapper.hpp>

#include "ir.hpp"
#include "plan.hpp"

namespace pipeline {

// Builds a plan on the device: one buffer per resource, filled from the document;
// per pass a compiled shader, one layout and one bind group per group index, and
// the pipeline. Nothing here decides anything; plan.hpp already did.
//
// Not yet: reusing a bind group across passes whose layouts come out identical.
// Each pass gets its own, which is correct and only wasteful.
//
// The escape hatch runs both ways (PIPELINE_DSL.md, "the backdoor is two-way"):
// - out: buffer(name) is the live buffer, and encode() records the passes into
//   an encoder the host owns, beside whatever else the host records;
// - in: a buffer the host made can stand in for a storage resource. The document
//   still declares it; the loader binds the host's buffer, never writes to it,
//   and sizes whatever derives from it by the host's buffer (make_plan's
//   `adopted`, which adopted_lengths() fills).
// Neither switches anything else off: readback, derived sizes and binding all
// carry on around a resource the host owns.
//
// adoption_errors() checks an adopted buffer carries the usages its role needs
// before anything is bound, rather than leaving it to a WebGPU validation error.

using values = std::map<std::string, std::vector<double>>;
// What execute() hands back: the outputs, or no values and what went wrong.
using result_callback = std::function<void(values, std::vector<std::string> errors)>;
using adopted_buffers = std::map<std::string, dawn_wrapper::buffer_wrapper>;

// Every adopted buffer needs Storage to be bound. One a pass writes also needs
// CopySrc if execute() is to read it back; a host that only uses encode() and
// copies nothing out can pass `read_back = false`.
inline std::vector<std::string> adoption_errors(const plan& p, const adopted_buffers& adopted, bool read_back = true)
{
    std::vector<std::string> errors;
    for (const auto& [name, buffer] : adopted) {
        const auto usage = buffer.get_usage();
        if (!usage.storage) {
            errors.push_back("adopted '" + name + "' was not made with Storage usage, so it cannot be bound");
        }
        const bool output = std::find(p.outputs.begin(), p.outputs.end(), name) != p.outputs.end();
        if (read_back && output && !usage.copy_src) {
            errors.push_back("adopted '" + name + "' is written by a pass but has no CopySrc usage, so execute() cannot read it back");
        }
        if (buffer.get_size() % 4) {
            errors.push_back("adopted '" + name + "' is " + std::to_string(buffer.get_size()) + " bytes, not a whole number of 4-byte elements");
        }
    }
    return errors;
}

// Element counts for make_plan: every storage type here is four bytes.
inline std::map<std::string, unsigned> adopted_lengths(const adopted_buffers& adopted)
{
    std::map<std::string, unsigned> lengths;
    for (const auto& [name, buffer] : adopted) {
        lengths[name] = unsigned(buffer.get_size() / 4);
    }
    return lengths;
}

namespace detail {

    inline void put(std::vector<uint8_t>& out, size_t offset, const std::string& type, double v)
    {
        if (type == "f32") {
            const float f = float(v);
            std::memcpy(out.data() + offset, &f, 4);
        } else if (type == "u32") {
            const uint32_t u = uint32_t(v);
            std::memcpy(out.data() + offset, &u, 4);
        } else {
            const int32_t i = int32_t(v);
            std::memcpy(out.data() + offset, &i, 4);
        }
    }

    inline double get(const uint8_t* in, size_t offset, const std::string& type)
    {
        if (type == "f32") {
            float f;
            std::memcpy(&f, in + offset, 4);
            return f;
        } else if (type == "u32") {
            uint32_t u;
            std::memcpy(&u, in + offset, 4);
            return u;
        }
        int32_t i;
        std::memcpy(&i, in + offset, 4);
        return i;
    }
}

struct instance {
    // `on_messages` hears each pass's compilation messages, late, as
    // compute_wrapper::compile_shader delivers them: (pass name, messages), and an
    // empty string means it compiled clean.
    instance(dawn_wrapper::dawn_plugin plugin, const document& doc, const plan& p,
        std::function<void(std::string, std::string)> on_messages,
        adopted_buffers adopted = {})
        : m_plugin(plugin)
        , m_plan(p)
        , m_build(std::make_shared<build_state>())
    {
        using namespace dawn_wrapper;

        // Everything built here is judged by one error scope, so execute() can
        // wait for the verdict instead of running an invalid pipeline and reading
        // back zeros: a compiled module whose `entry` names no function in it is
        // otherwise only an uncaptured error, reported after the fact.
        plugin.push_error_scope();

        for (const auto& r : doc.resources) {
            const auto size = p.bytes.at(r.name);
            std::vector<uint8_t> bytes(size, 0);
            if (const auto it = adopted.find(r.name); it != adopted.end()) {
                ASSERT(r.kind == resource_kind::storage);
                m_types[r.name] = r.element;
                m_buffers[r.name] = it->second;
            } else if (r.kind == resource_kind::storage) {
                for (size_t i = 0; i < r.init.size(); ++i) {
                    detail::put(bytes, i * 4, r.element, r.init[i]);
                }
                m_types[r.name] = r.element;
                m_buffers[r.name] = plugin.make_dst_buffer(size, buffer_type::copy).write(bytes.data());
            } else {
                for (size_t i = 0; i < r.members.size(); ++i) {
                    detail::put(bytes, i * 4, r.members[i].type, r.members[i].value);
                }
                m_buffers[r.name] = plugin.make_dst_buffer(size, buffer_type::uniform).write(bytes.data());
            }
        }

        for (const auto& pp : p.passes) {
            auto compute = plugin.make_compute();
            compute.compile_shader(pp.wgsl, pp.entry, [on_messages, name = pp.name](std::string messages) {
                if (on_messages) {
                    on_messages(name, messages);
                }
            });

            std::vector<bindgroup_layout_wrapper> layouts;
            bindgroup_set set;
            for (const auto& bg : pp.groups) {
                auto layout = compute.make_bindgroup_layout();
                for (const auto& b : bg.bindings) {
                    switch (b.mode) {
                    case access::read:
                        layout.add_read_only_buffer(b.index);
                        break;
                    case access::read_write:
                        layout.add_buffer(b.index);
                        break;
                    case access::uniform:
                        layout.add_uniform_buffer(b.index);
                        break;
                    }
                }
                auto group = layout.make_bindgroup();
                for (const auto& b : bg.bindings) {
                    group.add_buffer(b.index, m_buffers.at(b.resource));
                }
                layouts.push_back(layout);
                set.add_bindgroup(group, bg.index);
            }
            compute.init_pipeline(layouts);
            m_passes.push_back({ compute, set, pp.workgroups });
        }

        plugin.pop_error_scope([build = m_build](std::string error) {
            build->error = error;
            build->resolved = true;
            auto waiting = std::move(build->waiting);
            for (auto& run : waiting) {
                run();
            }
        });
    }

    // The live buffer behind a resource: the loader's own, or the one adopted.
    dawn_wrapper::buffer_wrapper buffer(const std::string& name) const
    {
        return m_buffers.at(name);
    }

    // Records every pass, in document order, into the host's encoder. Submitting
    // it is the host's business.
    void encode(dawn_wrapper::encoder_wrapper encoder)
    {
        for (auto& pass : m_passes) {
            pass.compute.compute(pass.set, pass.workgroups, 1, encoder);
        }
    }

    // Runs every pass in document order in one submission, then reads back every
    // resource a pass writes. `done` is called once, after the build's error scope
    // has resolved, the run's own has, and the last output has mapped; with any
    // error it gets no values, since what mapped is not a result.
    //
    // Holds `this` until then when the build has not resolved yet: the instance
    // must outlive the call, as it must for the readback anyway.
    void execute(result_callback done)
    {
        using namespace dawn_wrapper;

        if (!m_build->resolved) {
            m_build->waiting.push_back([this, done] { execute(done); });
            return;
        }
        if (!m_build->error.empty()) {
            done({}, { "building: " + m_build->error });
            return;
        }

        m_plugin.push_error_scope();
        auto encoder = m_plugin.make_encoder();
        encode(encoder);

        std::vector<std::pair<std::string, buffer_wrapper>> staging;
        for (const auto& name : m_plan.outputs) {
            auto mapped = m_plugin.make_dst_buffer(m_plan.bytes.at(name), buffer_type::map_read);
            encoder.copy_buffer_to_buffer(m_buffers.at(name), mapped);
            staging.emplace_back(name, mapped);
        }
        encoder.submit_command_buffer();

        struct pending {
            values result;
            std::vector<std::string> errors;
            size_t left;
            result_callback done;

            void one_more()
            {
                if (--left == 0) {
                    done(errors.empty() ? result : values {}, errors);
                }
            }
        };
        // Every output's map, and the scope.
        auto state = std::make_shared<pending>(pending { {}, {}, staging.size() + 1, done });

        m_plugin.pop_error_scope([state](std::string error) {
            if (!error.empty()) {
                state->errors.push_back("running: " + error);
            }
            state->one_more();
        });
        for (auto& [name, mapped] : staging) {
            mapped.get_output([state, name, type = m_types.at(name)](size_t size, const void* data) {
                auto& out = state->result[name];
                const auto* bytes = static_cast<const uint8_t*>(data);
                for (size_t offset = 0; bytes && offset + 4 <= size; offset += 4) {
                    out.push_back(detail::get(bytes, offset, type));
                }
                if (!bytes) {
                    state->errors.push_back("reading back '" + name + "': the map failed");
                }
                state->one_more();
            });
        }
    }

private:
    struct compiled_pass {
        dawn_wrapper::compute_wrapper compute;
        dawn_wrapper::bindgroup_set set;
        unsigned workgroups;
    };

    dawn_wrapper::dawn_plugin m_plugin;
    plan m_plan;
    std::map<std::string, dawn_wrapper::buffer_wrapper> m_buffers;
    std::map<std::string, std::string> m_types;
    std::vector<compiled_pass> m_passes;

    struct build_state {
        bool resolved = false;
        std::string error;
        std::vector<std::function<void()>> waiting;
    };
    std::shared_ptr<build_state> m_build;
};

}
