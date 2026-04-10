#pragma once

#include "buffer_wrapper_impl.hpp"
#include "texture_output_wrapper_impl.hpp"
#include "texture_wrapper_impl.hpp"

#include <map>
#include <string>

using namespace wgpu;

namespace dawn_wrapper {

struct bindgroup_set::pimpl {
    
    void add_bindgroup(bindgroup_wrapper bg, unsigned group)
    {
        m_bindgroups[group] = bg;
    }

    pimpl()
        : m_bindgroups()
    {
    }

    std::map<unsigned, bindgroup_wrapper> m_bindgroups;
};

}
