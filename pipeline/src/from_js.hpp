#pragma once

#include <string>
#include <vector>

#include <emscripten/val.h>

#include "ir.hpp"

namespace pipeline {

// A parsed document (JSON.parse's output, handed over as a val) into the IR. JS
// has already read the text and fetched each pass's `wgsl` files into `source`;
// see pre.js. Anything missing or the wrong shape is left at its
// default and make_plan says what is wrong with it, so this does no validation of
// its own beyond the shape it can read.

namespace detail {
    using emscripten::val;

    inline std::vector<std::string> keys(val object)
    {
        if (!object.isUndefined() && !object.isNull()) {
            return emscripten::vecFromJSArray<std::string>(val::global("Object").call<val>("keys", object));
        }
        return {};
    }

    inline std::string string_or(val v, std::string otherwise = "")
    {
        return v.isString() ? v.as<std::string>() : otherwise;
    }

    inline std::vector<std::string> strings(val v)
    {
        return val::global("Array").call<bool>("isArray", v) ? emscripten::vecFromJSArray<std::string>(v) : std::vector<std::string> {};
    }
}

inline document from_js(emscripten::val doc)
{
    using namespace detail;

    document result;

    const auto resources = doc["resources"];
    for (const auto& name : keys(resources)) {
        const auto r = resources[name];
        resource out;
        out.name = name;
        const auto kind = string_or(r["kind"]);
        out.kind = kind == "uniform" ? resource_kind::uniform : resource_kind::storage;
        out.element = string_or(r["element"], "f32");
        if (val::global("Array").call<bool>("isArray", r["init"])) {
            out.init = emscripten::vecFromJSArray<double>(r["init"]);
        }
        const auto length = r["length"];
        if (length.isNumber()) {
            out.length = length.as<unsigned>();
        } else if (length.isString()) {
            out.length_of = length.as<std::string>();
        }
        const auto members = r["members"];
        for (const auto& member_name : keys(members)) {
            const auto m = members[member_name];
            out.members.push_back({ member_name, string_or(m["type"]), m["value"].isNumber() ? m["value"].as<double>() : 0 });
        }
        result.resources.push_back(out);
    }

    const auto groups = doc["groups"];
    for (const auto& name : keys(groups)) {
        result.groups.push_back({ name, strings(groups[name]) });
    }

    const auto passes = doc["passes"];
    const auto count = val::global("Array").call<bool>("isArray", passes) ? passes["length"].as<unsigned>() : 0;
    for (unsigned i = 0; i < count; ++i) {
        const auto p = passes[i];
        pass out;
        out.name = string_or(p["name"], "pass " + std::to_string(i));
        out.reads = strings(p["reads"]);
        out.writes = strings(p["writes"]);
        if (p["workgroup_size"].isNumber()) {
            out.workgroup_size = p["workgroup_size"].as<unsigned>();
        }
        out.dispatch_over = string_or(p["dispatch"]["over"]);
        out.entry = string_or(p["entry"]);
        out.source = string_or(p["source"]);
        result.passes.push_back(out);
    }

    return result;
}

}
