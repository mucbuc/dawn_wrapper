#pragma once

#include "dawn_utils.hpp"

using namespace wgpu;

namespace dawn_wrapper {
struct texture_wrapper::pimpl {
    pimpl() = default;

    pimpl(Device device, const std::vector<uint8_t>& colors)
        : m_device(device)
        , m_width(unsigned(colors.size()) / 4)
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

    // Part of it: `width` by `height` texels at (x, y), inside the texture.
    void write_region(unsigned x, unsigned y, unsigned width, unsigned height, const std::vector<uint8_t>& rgba)
    {
        ASSERT(x + width <= m_desc.size.width && y + height <= m_desc.size.height);
        ASSERT(rgba.size() == size_t(4) * width * height);
        dawn_utils::write_texture_region(m_device, m_texture, x, y, width, height, rgba);
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

    unsigned get_width() const
    {
        return m_width;
    }

    unsigned get_height() const
    {
        return m_height;
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
        TexelCopyTextureInfo destination {};
        destination.texture = texture;
        destination.mipLevel = 0;
        destination.origin = { 0, 0, 0 };
        destination.aspect = TextureAspect::All;

        TexelCopyBufferLayout source {};
        source.offset = 0;
        source.bytesPerRow = 4 * desc.size.width;
        source.rowsPerImage = desc.size.height;

        device.GetQueue().WriteTexture(&destination, colorTexture.data(), colorTexture.size(), &source, &desc.size);
    }
};

}
