#pragma once

#include "buffer_wrapper_impl.h"
#include "texture_output_wrapper_impl.h"
#include "texture_wrapper_impl.h"

using namespace wgpu;

namespace dawn_wrapper {

struct bindgroup_wrapper::pimpl {
    void add_buffer(unsigned binding, buffer_wrapper buffer)
    {
        m_bindgroup_entries.push_back(dawn_utils::make_bindGroupBufferEntry(binding, buffer.m_pimpl->m_buffer));
    }

    void addTexture(unsigned binding, texture_wrapper texture)
    {
        m_bindgroup_entries.push_back(dawn_utils::make_bind_group_entry(binding, texture.m_pimpl->get_view()));
    }

    void addTexture(unsigned binding, texture_output_wrapper texture)
    {
        m_bindgroup_entries.push_back(dawn_utils::make_bind_group_entry(binding, texture.m_pimpl->get_view()));
    }

    void add_sampler(unsigned binding, texture_wrapper texture)
    {
        m_bindgroup_entries.push_back(dawn_utils::make_bind_group_entry(binding, texture.m_pimpl->get_sampler()));
    }

    void add_sampler(unsigned binding, texture_output_wrapper texture)
    {
        m_bindgroup_entries.push_back(dawn_utils::make_bind_group_entry(binding, texture.m_pimpl->get_sampler()));
    }

    BindGroup make_bindgroup(Device device, BindGroupLayout layout)
    {
        return dawn_utils::make_bindGroup(device, layout, m_bindgroup_entries, m_context_name.c_str());
    }

    pimpl(std::string context_name)
        : m_bindgroup_entries()
        , m_context_name(context_name)
    {
    }

private:
    std::vector<BindGroupEntry> m_bindgroup_entries;
    std::string m_context_name;
};

}
