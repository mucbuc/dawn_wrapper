#pragma once

#include "buffer_wrapper_impl.hpp"
#include "texture_output_wrapper_impl.hpp"
#include "texture_wrapper_impl.hpp"

using namespace wgpu;

namespace dawn_wrapper {

struct bindgroup_set::pimpl {
    
    void add_bindgroup(bindgroup_wrapper bg, unsigned group)
    {
        m_bindgroups[group] = bg;
    }

    pimpl(std::string context_name)
        : m_bindgroups()
        , m_context_name(context_name)
    {
    }

private:
    std::map<unsigned, bindgroup_wrapper> m_bindgroups;
    std::string m_context_name;
};

}
