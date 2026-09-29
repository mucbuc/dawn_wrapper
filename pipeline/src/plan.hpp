#pragma once

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "ir.hpp"

namespace pipeline {

// Everything the document implies, worked out without a GPU: lengths, byte sizes,
// group and binding numbers, access per pass, the generated WGSL and the dispatch
// count. The loader only acts on this; a document that fails here never reaches
// the device.

enum class access {
    read, // var<storage, read>
    read_write, // var<storage, read_write>
    uniform, // var<uniform>
};

struct binding {
    std::string resource;
    unsigned index = 0;
    access mode = access::read;
};

// Empty `bindings` is a gap: the pass uses a later group but not this one, and
// WebGPU still wants something bound at every index up to the last.
struct bound_group {
    unsigned index = 0;
    std::string name;
    std::vector<binding> bindings;
};

struct pass_plan {
    std::string name;
    std::string entry;
    // The generated header followed by the pass's source: what gets compiled.
    std::string wgsl;
    std::vector<bound_group> groups;
    unsigned workgroups = 0;
};

struct plan {
    std::vector<std::string> errors;
    std::map<std::string, unsigned> lengths; // storage: element count
    std::map<std::string, unsigned> bytes; // every resource: buffer size
    std::vector<pass_plan> passes;
    // Resources some pass writes, in document order: what a run reads back.
    std::vector<std::string> outputs;

    bool ok() const { return errors.empty(); }
};

namespace detail {

    // WebGPU's default maxBindGroups.
    constexpr unsigned max_groups = 4;

    inline bool is_scalar(const std::string& type)
    {
        return type == "f32" || type == "u32" || type == "i32";
    }

    inline std::string struct_name(const std::string& resource)
    {
        std::string s = resource;
        if (!s.empty()) {
            s[0] = char(std::toupper(static_cast<unsigned char>(s[0])));
        }
        return s;
    }

    inline bool contains(const std::vector<std::string>& v, const std::string& s)
    {
        return std::find(v.begin(), v.end(), s) != v.end();
    }
}

// `adopted` names storage resources whose buffer the host supplies, with that
// buffer's element count. The count replaces whatever the document says (init is
// not written into a buffer the host owns), and anything deriving its length from
// an adopted resource follows the host's buffer. See loader.hpp.
inline plan make_plan(const document& doc, const std::map<std::string, unsigned>& adopted = {})
{
    using namespace detail;

    plan result;
    auto fail = [&](std::string message) { result.errors.push_back(std::move(message)); };

    std::map<std::string, const resource*> by_name;
    for (const auto& r : doc.resources) {
        if (r.name.empty()) {
            fail("a resource has no name");
        } else if (!by_name.emplace(r.name, &r).second) {
            fail("resource '" + r.name + "' is declared twice");
        }

        if (r.kind == resource_kind::storage) {
            if (!is_scalar(r.element)) {
                fail("storage '" + r.name + "': element '" + r.element + "' is not f32, u32 or i32");
            }
            if (!r.members.empty()) {
                fail("storage '" + r.name + "' has members; only a uniform does");
            }
        } else {
            if (r.members.empty()) {
                fail("uniform '" + r.name + "' has no members");
            }
            for (const auto& m : r.members) {
                if (!is_scalar(m.type)) {
                    fail("uniform '" + r.name + "': member '" + m.name + "' type '" + m.type + "' is not f32, u32 or i32");
                }
            }
        }
    }

    for (const auto& [name, n] : adopted) {
        const auto it = by_name.find(name);
        if (it == by_name.end() || it->second->kind != resource_kind::storage) {
            fail("adopts '" + name + "', which is not a storage resource in the document");
        } else if (n == 0) {
            fail("adopts '" + name + "' with an empty buffer");
        }
    }

    // Lengths, following length_of chains; a cycle or a dangling name is an error.
    std::map<std::string, unsigned> lengths;
    auto resolve = [&](const resource& start) -> unsigned {
        std::set<std::string> seen;
        const resource* r = &start;
        while (!adopted.count(r->name) && r->length == 0 && !r->length_of.empty()) {
            if (!seen.insert(r->name).second) {
                fail("storage '" + start.name + "': length_of forms a cycle");
                return 0;
            }
            const auto it = by_name.find(r->length_of);
            if (it == by_name.end() || it->second->kind != resource_kind::storage) {
                fail("storage '" + r->name + "': length_of '" + r->length_of + "' is not a storage resource");
                return 0;
            }
            r = it->second;
        }
        if (const auto it = adopted.find(r->name); it != adopted.end()) {
            return it->second;
        }
        return r->length ? r->length : unsigned(r->init.size());
    };

    for (const auto& r : doc.resources) {
        if (r.kind == resource_kind::storage) {
            const auto n = resolve(r);
            if (n == 0) {
                fail("storage '" + r.name + "' has no length: give length, length_of or init");
            } else if (!adopted.count(r.name) && r.init.size() > n) {
                fail("storage '" + r.name + "': init has " + std::to_string(r.init.size()) + " values for a length of " + std::to_string(n));
            }
            lengths[r.name] = n;
            result.bytes[r.name] = n * 4;
        } else {
            // Rounded up to 16: harmless for scalars, and what a uniform holding a
            // vec4 or a nested struct will need anyway.
            const unsigned raw = unsigned(r.members.size()) * 4;
            result.bytes[r.name] = (raw + 15) / 16 * 16;
        }
    }
    result.lengths = lengths;

    // Groups: declared, or one implicit group of everything.
    std::vector<group> groups = doc.groups;
    if (groups.empty()) {
        group all { "default", {} };
        for (const auto& r : doc.resources) {
            all.resources.push_back(r.name);
        }
        groups.push_back(all);
    }
    if (groups.size() > max_groups) {
        fail(std::to_string(groups.size()) + " groups; WebGPU guarantees " + std::to_string(max_groups));
    }

    struct place {
        unsigned group;
        unsigned binding;
    };
    std::map<std::string, place> placed;
    std::set<std::string> group_names;
    for (unsigned g = 0; g < groups.size(); ++g) {
        if (!group_names.insert(groups[g].name).second) {
            fail("group '" + groups[g].name + "' is declared twice");
        }
        for (unsigned b = 0; b < groups[g].resources.size(); ++b) {
            const auto& name = groups[g].resources[b];
            if (!by_name.count(name)) {
                fail("group '" + groups[g].name + "' names '" + name + "', which is not a resource");
            } else if (!placed.emplace(name, place { g, b }).second) {
                fail("resource '" + name + "' is in more than one group");
            }
        }
    }
    for (const auto& r : doc.resources) {
        if (!placed.count(r.name)) {
            fail("resource '" + r.name + "' is in no group");
        }
    }

    for (const auto& r : doc.resources) {
        for (const auto& p : doc.passes) {
            if (contains(p.writes, r.name)) {
                result.outputs.push_back(r.name);
                break;
            }
        }
    }

    if (doc.passes.empty()) {
        fail("the document has no passes");
    }

    for (const auto& p : doc.passes) {
        const std::string where = "pass '" + p.name + "'";

        // Access per pass: written means read_write even when also read.
        std::map<std::string, access> used;
        for (const auto& name : p.reads) {
            const auto it = by_name.find(name);
            if (it == by_name.end()) {
                fail(where + " reads '" + name + "', which is not a resource");
                continue;
            }
            used[name] = it->second->kind == resource_kind::uniform ? access::uniform : access::read;
        }
        for (const auto& name : p.writes) {
            const auto it = by_name.find(name);
            if (it == by_name.end()) {
                fail(where + " writes '" + name + "', which is not a resource");
                continue;
            }
            if (it->second->kind == resource_kind::uniform) {
                fail(where + " writes uniform '" + name + "'; a uniform is read-only in a shader");
                continue;
            }
            used[name] = access::read_write;
        }

        if (p.workgroup_size == 0) {
            fail(where + ": workgroup_size is 0");
        }
        if (p.entry.empty()) {
            fail(where + " names no entry point");
        }
        if (p.source.empty()) {
            fail(where + " has no WGSL");
        }

        unsigned workgroups = 0;
        const auto over = lengths.find(p.dispatch_over);
        if (over == lengths.end()) {
            fail(where + ": dispatch over '" + p.dispatch_over + "', which is not a storage resource");
        } else if (p.workgroup_size) {
            workgroups = (over->second + p.workgroup_size - 1) / p.workgroup_size;
            if (workgroups >= 65535) {
                fail(where + ": " + std::to_string(workgroups) + " workgroups, over the 65535 a dimension allows");
            }
        }

        if (!result.ok()) {
            continue;
        }

        // Groups up to the highest one this pass touches, gaps included.
        unsigned last = 0;
        for (const auto& [name, mode] : used) {
            last = std::max(last, placed[name].group + 1);
        }
        pass_plan pp { p.name, p.entry, "", {}, workgroups };
        for (unsigned g = 0; g < last; ++g) {
            bound_group bg { g, groups[g].name, {} };
            for (const auto& name : groups[g].resources) {
                const auto it = used.find(name);
                if (it != used.end()) {
                    bg.bindings.push_back({ name, placed[name].binding, it->second });
                }
            }
            pp.groups.push_back(bg);
        }

        std::stringstream wgsl;
        for (const auto& [name, mode] : used) {
            if (mode != access::uniform) {
                continue;
            }
            wgsl << "struct " << struct_name(name) << " {\n";
            for (const auto& m : by_name[name]->members) {
                wgsl << "    " << m.name << ": " << m.type << ",\n";
            }
            wgsl << "}\n";
        }
        for (const auto& bg : pp.groups) {
            for (const auto& b : bg.bindings) {
                const auto& r = *by_name[b.resource];
                wgsl << "@group(" << bg.index << ") @binding(" << b.index << ") var";
                switch (b.mode) {
                case access::read:
                    wgsl << "<storage, read> " << r.name << ": array<" << r.element << ">;\n";
                    break;
                case access::read_write:
                    wgsl << "<storage, read_write> " << r.name << ": array<" << r.element << ">;\n";
                    break;
                case access::uniform:
                    wgsl << "<uniform> " << r.name << ": " << struct_name(r.name) << ";\n";
                    break;
                }
            }
        }
        wgsl << "const WORKGROUP_SIZE: u32 = " << p.workgroup_size << "u;\n\n"
             << p.source;
        pp.wgsl = wgsl.str();

        result.passes.push_back(pp);
    }

    return result;
}

}
