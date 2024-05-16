#pragma once

#include "dawn_utils.hpp"
#include "dawn_wrapper.h"

#include "buffer_wrapper_impl.h"
#include "texture_wrapper_impl.h"
#include "texture_output_wrapper_impl.h"

#include <dawn/webgpu_cpp.h>

namespace dawn_wrapper {

struct bindgroup_wrapper::pimpl {
    void addBuffer(unsigned binding, buffer_wrapper buffer)
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

    void addSampler(unsigned binding, texture_wrapper texture)
    {
        m_bindgroup_entries.push_back(dawn_utils::make_bind_group_entry(binding, texture.m_pimpl->get_sampler()));
    }

    void addSampler(unsigned binding, texture_output_wrapper texture)
    {
        m_bindgroup_entries.push_back(dawn_utils::make_bind_group_entry(binding, texture.m_pimpl->get_sampler()));
    }

    BindGroup make_bindgroup(Device device, BindGroupLayout layout)
    {
        return dawn_utils::make_bindGroup(device, layout, m_bindgroup_entries);
    }

    pimpl() = default;

    std::vector<BindGroupEntry> m_bindgroup_entries;
};

}
