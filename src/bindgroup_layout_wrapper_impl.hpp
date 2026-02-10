#pragma once

#include "dawn_utils.hpp"

using namespace wgpu;

namespace dawn_wrapper {

struct bindgroup_layout_wrapper::pimpl {
    pimpl(ShaderStage stage)
        : m_stage(stage)
    {
    }

    void add_buffer(unsigned binding, bool enable)
    {
        if (enable) {
            m_layoutEntries.push_back(dawn_utils::make_bindGroupLayoutBufferEntry(binding, BufferBindingType::Storage, m_stage));
        }
    }

    void add_read_only_buffer(unsigned binding, bool enable)
    {
        if (enable) {
            m_layoutEntries.push_back(dawn_utils::make_bindGroupLayoutBufferEntry(binding, BufferBindingType::ReadOnlyStorage, m_stage));
        }
    }

    void add_uniform_buffer(unsigned binding, bool enable)
    {
        if (enable) {
            m_layoutEntries.push_back(dawn_utils::make_bindGroupLayoutBufferEntry(binding, BufferBindingType::Uniform, m_stage));
        }
    }

    void add_texture_1d(unsigned binding, bool enable)
    {
        if (enable) {
            m_layoutEntries.push_back(make_texture_layout_entry(binding, TextureSampleType::Float, TextureViewDimension::e1D));
        }
    }

    void add_texture_2d(unsigned binding, bool enable)
    {
        if (enable) {
            m_layoutEntries.push_back(make_texture_layout_entry(binding, TextureSampleType::Float, TextureViewDimension::e2D));
        }
    }

    void add_storage_texture_2d(unsigned binding, bool enable)
    {
        if (enable) {
            m_layoutEntries.push_back(make_texture_output_layout_entry(binding));
        }
    }

    void add_sampler(unsigned binding, bool enable)
    {
        if (enable) {
            m_layoutEntries.push_back(make_sampler_layout_entry(binding));
        }
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

    BindGroupLayoutEntry make_texture_output_layout_entry(unsigned binding, TextureSampleType type = TextureSampleType::Float, TextureViewDimension dimension = TextureViewDimension::e2D)
    {
        BindGroupLayoutEntry entry = {};
        entry.binding = binding;
        entry.storageTexture.access = StorageTextureAccess::WriteOnly;
        entry.storageTexture.format = TextureFormat::RGBA8Unorm;
        entry.storageTexture.viewDimension = dimension;
        entry.visibility = m_stage;
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
