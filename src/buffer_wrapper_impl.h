#pragma once

#include "dawn_utils.hpp"
#include "dawn_wrapper.h"

#include <dawn/webgpu_cpp.h>

namespace dawn_wrapper {
struct buffer_wrapper::pimpl {
    friend class compute_wrapper::pimpl;

    pimpl() = default;

    pimpl(Device device, unsigned size, BufferType flags, bool isDest)
        : m_device(device)
        , m_isDest(isDest)
        , m_usage(getBufferUsageFromType(flags, m_isDest))
        , m_buffer(dawn_utils::make_buffer(m_device, size, m_usage))
    {
    }

    pimpl(Device device, BufferType flags, bool isDest)
        : m_device(device)
        , m_isDest(isDest)
        , m_usage(getBufferUsageFromType(flags, m_isDest))
        , m_buffer()
    {
    }

    void write(void* data, size_t size)
    {
        if (!m_buffer || m_buffer.GetSize() < size) {
            m_buffer = dawn_utils::make_buffer(m_device, size, m_usage);
        }

        m_device.GetQueue().WriteBuffer(m_buffer, 0, data, size);
    }

    void write(const std::vector<uint8_t>& colors)
    {
        write((void*)colors.data(), colors.size());
    }

    // private:

    static BufferUsage getBufferUsageFromType(BufferType type, bool isDest)
    {
        BufferUsage flags = isDest ? BufferUsage::CopyDst : BufferUsage::CopySrc;
        switch (type) {
        case BufferType::Uniform:
            flags |= BufferUsage::Uniform;
            break;

        case BufferType::Storage:
            flags |= BufferUsage::Storage;
            break;

        case BufferType::Index:
            flags |= BufferUsage::Index;
            break;

        case BufferType::Vertex:
            flags |= BufferUsage::Vertex;
            break;

        default:
            ASSERT(false);
        }

        return flags;
    }

    Device m_device;
    bool m_isDest;
    BufferUsage m_usage;
    Buffer m_buffer;
};
}
