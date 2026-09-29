#pragma once

#include <string>
#include <vector>

namespace pipeline {

// The in-memory form of a pipeline document. Whatever reads the text fills this
// in (JS in the browser today, see from_js.hpp); nothing from here down knows the
// text was JSON. See PIPELINE_DSL.md.

enum class resource_kind {
    storage,
    uniform,
};

// Scalars only for now: f32, u32 and i32 are all four bytes and four-aligned, so
// a uniform's layout is its member count times four. vecN and structs change that
// and are not handled yet.
struct member {
    std::string name;
    std::string type;
    double value = 0;
};

struct resource {
    std::string name;
    resource_kind kind = resource_kind::storage;

    // storage: an array of `element`. Its length is `length` if given, the length
    // of the resource named by `length_of` if given, and otherwise init's size.
    std::string element;
    std::vector<double> init;
    unsigned length = 0;
    std::string length_of;

    // uniform: one struct, members in document order.
    std::vector<member> members;
};

// A named bind group. Its index is its position in document::groups, and each
// resource's binding is its position in `resources`; nobody writes a number.
struct group {
    std::string name;
    std::vector<std::string> resources;
};

struct pass {
    std::string name;
    std::vector<std::string> reads;
    std::vector<std::string> writes;
    unsigned workgroup_size = 64;
    // A storage resource: one invocation per element.
    std::string dispatch_over;
    // The entry point, a whole function in `source`. Its @workgroup_size should
    // say WORKGROUP_SIZE, a const the generated header defines from
    // workgroup_size, so the number is stated once.
    std::string entry;
    // WGSL: the document's `wgsl` files, concatenated in order. Whole functions;
    // only the resource declarations in front of them are generated.
    std::string source;
};

struct document {
    std::vector<resource> resources;
    // Empty means one implicit group holding every resource.
    std::vector<group> groups;
    std::vector<pass> passes;
};

}
