#pragma once

#include "dawn_utils.hpp"
#include "dawn_wrapper.h"

#include <dawn/webgpu_cpp.h>

namespace dawn_wrapper {
struct texture_wrapper::pimpl {
    friend class render_wrapper::pimpl;

    pimpl() = default;

    pimpl(Device device, const std::vector<uint8_t>& colors)
        : m_device(device)
        , m_width(colors.size() / 4)
        , m_height(1)
        , m_desc(dawn_utils::make_texture_descriptor(m_device, m_width))
        , m_texture(m_device.CreateTexture(&m_desc))
        , m_view(m_texture.CreateView())
    {
        write(colors);
    }

    pimpl(Device device, unsigned size)
        : m_device(device)
        , m_width(size)
        , m_height(1)
        , m_desc(dawn_utils::make_texture_descriptor(m_device, m_width))
        , m_texture(m_device.CreateTexture(&m_desc))
        , m_view(m_texture.CreateView())
    {
    }

    pimpl(Device device, unsigned width, unsigned height)
        : m_device(device)
        , m_width(width)
        , m_height(height)
        , m_desc(dawn_utils::make_texture_descriptor_2d(m_device, m_width, m_height))
        , m_texture(m_device.CreateTexture(&m_desc))
        , m_view(m_texture.CreateView())
    {
    }

    void write(const std::vector<uint8_t>& colors)
    {
        ASSERT(colors.size() == 4 * m_desc.size.width * m_desc.size.height);

        write_texture(m_device, m_texture, m_desc, colors);
    }

    TextureView get_view()
    {
        ASSERT(m_texture);
        return m_view;
    }

    void make_sampler(bool clamp_to_edge)
    {
        m_sampler = clamp_to_edge ? dawn_utils::make_sampler(m_device, AddressMode::ClampToEdge) : dawn_utils::make_sampler(m_device, AddressMode::Repeat);
    }

    Sampler get_sampler()
    {
        return m_sampler;
    }

private:
    Device m_device;
    unsigned m_width;
    unsigned m_height;
    TextureDescriptor m_desc;
    Texture m_texture;
    TextureView m_view;
    Sampler m_sampler;

    static void write_texture(Device& device, Texture& texture, TextureDescriptor& desc, std::vector<uint8_t> colorTexture)
    {
        ImageCopyTexture destination {};
        destination.texture = texture;
        destination.mipLevel = 0;
        destination.origin = { 0, 0, 0 };
        destination.aspect = TextureAspect::All;

        TextureDataLayout source {};
        source.offset = 0;
        source.bytesPerRow = 4 * desc.size.width;
        source.rowsPerImage = desc.size.height;

        device.GetQueue().WriteTexture(&destination, colorTexture.data(), colorTexture.size(), &source, &desc.size);
    }
};

}
