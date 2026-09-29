#pragma once

#include "buffer_wrapper_impl.hpp"
#include "texture_output_wrapper_impl.hpp"

using namespace wgpu;

namespace dawn_wrapper {
struct encoder_wrapper::pimpl {
    pimpl() = default;

    pimpl(Device device)
        : m_device(device)
        , m_encoder(m_device.CreateCommandEncoder())
    {
    }

    void submit_command_buffer()
    {
        CommandBuffer commands = m_encoder.Finish();
        m_encoder = m_device.CreateCommandEncoder();
        m_device.GetQueue().Submit(1, &commands);
    }

    void copy_buffer_to_buffer(buffer_wrapper source, buffer_wrapper destination, size_t offset)
    {
        m_encoder.CopyBufferToBuffer(source.m_pimpl->m_buffer, 0, destination.m_pimpl->m_buffer, offset, source.m_pimpl->m_buffer.GetSize());
    }

    void copy_texture_to_buffer(texture_output_wrapper texture, buffer_wrapper destination)
    {
        auto& t = *texture.m_pimpl;
        ASSERT(destination.get_size() >= t.bytes_per_row() * t.get_height());

        TexelCopyTextureInfo source {};
        source.texture = t.get_texture();
        source.mipLevel = 0;
        source.origin = { 0, 0, 0 };
        source.aspect = TextureAspect::All;

        TexelCopyBufferInfo target {};
        target.buffer = destination.m_pimpl->m_buffer;
        target.layout.offset = 0;
        target.layout.bytesPerRow = uint32_t(t.bytes_per_row());
        target.layout.rowsPerImage = t.get_height();

        Extent3D size { t.get_width(), t.get_height(), 1 };
        m_encoder.CopyTextureToBuffer(&source, &target, &size);
    }

    Device m_device;
    CommandEncoder m_encoder;
};
}
