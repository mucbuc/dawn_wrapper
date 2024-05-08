#pragma once

#include "dawn_utils.hpp"
#include "dawn_wrapper.h"

#include <dawn/webgpu_cpp.h>

namespace dawn_wrapper {

struct bindgroup_layout_wrapper::pimpl {
    pimpl(ShaderStage stage)
        : m_stage(stage)
    {
    }

    void addBuffer(unsigned binding)
    {
        m_layoutEntries.push_back(dawn_utils::make_bindGroupLayoutBufferEntry(binding, BufferBindingType::Storage, m_stage));
    }
    void addReadOnlyBuffer(unsigned binding)
    {
        m_layoutEntries.push_back(dawn_utils::make_bindGroupLayoutBufferEntry(binding, BufferBindingType::ReadOnlyStorage, m_stage));
    }

    void addUniformBuffer(unsigned binding)
    {
        m_layoutEntries.push_back(dawn_utils::make_bindGroupLayoutBufferEntry(binding, BufferBindingType::Uniform, m_stage));
    }

    void addTexture_1d(unsigned binding)
    {
        m_layoutEntries.push_back(make_texture_layout_entry(binding, TextureSampleType::Float, TextureViewDimension::e1D));
    }

    void addTexture_2d(unsigned binding)
    {
        m_layoutEntries.push_back(make_texture_layout_entry(binding, TextureSampleType::Float, TextureViewDimension::e2D));
    }

    void addSampler(unsigned binding)
    {
        m_layoutEntries.push_back(make_sampler_layout_entry(binding));
    }

    BindGroupLayoutEntry make_texture_layout_entry(unsigned binding, TextureSampleType type = TextureSampleType::Float, TextureViewDimension dimension = TextureViewDimension::e1D)
    {

        BindGroupLayoutEntry entry = {};
        entry.binding = binding;
        entry.visibility = m_stage;
        entry.texture.sampleType = type;
        entry.texture.viewDimension = dimension;
        return entry;
    }

    BindGroupLayoutEntry make_sampler_layout_entry(unsigned binding, TextureSampleType type = TextureSampleType::Float, TextureViewDimension dimension = TextureViewDimension::e1D)
    {

        BindGroupLayoutEntry entry = {};
        entry.binding = binding;
        entry.visibility = m_stage;
        entry.sampler.type = SamplerBindingType::Filtering;

        return entry;
    }

    ShaderStage m_stage;
    std::vector<BindGroupLayoutEntry> m_layoutEntries;
};

}
