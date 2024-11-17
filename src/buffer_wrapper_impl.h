#pragma once

#include "dawn_utils.hpp"

namespace dawn_wrapper {
struct buffer_wrapper::pimpl
    : public std::enable_shared_from_this<pimpl> {
    friend class compute_wrapper::pimpl;

    pimpl() = default;

    pimpl(Device device, unsigned size, buffer_type flags, bool isDest)
        : m_device(device)
        , m_size(size)
        , m_usage(getBufferUsageFromType(flags, isDest))
        , m_buffer(dawn_utils::make_buffer(m_device, m_size, m_usage))
        , m_done(true)
    {
    }

    void write(const void* data)
    {
        ASSERT(m_buffer);

        m_device.GetQueue().WriteBuffer(m_buffer, 0, data, m_size);
    }

    void write(const std::vector<uint8_t>& colors)
    {
        ASSERT(colors.size() == m_size);

        write(colors.data());
    }

    static void callback2(auto status, auto userData)
    {
        pimpl* instance = (pimpl*)userData;
        if (status == WGPUBufferMapAsyncStatus_Success) {
            const auto size = instance->m_buffer.GetSize();
            instance->m_dataCallback(size, instance->m_buffer.GetConstMappedRange(0, size));
            instance->m_buffer.Unmap();
        } else {
            instance->m_dataCallback(0, nullptr);
        }
        instance->m_self_ref.reset();
        instance->m_done = true;
    }

    void get_output(std::function<void(unsigned, const void*)> cb)
    {
        m_dataCallback = cb;

        auto buffer_size = m_buffer.GetSize();
        m_done = false;

        m_self_ref = shared_from_this();
        m_buffer.MapAsync(MapMode::Read, 0, buffer_size, &callback2, this);
    }

    bool done()
    {
        return m_done;
    }

    unsigned long get_size()
    {
        return m_buffer.GetSize();
    }

    // private:

    static BufferUsage getBufferUsageFromType(buffer_type type, bool isDest)
    {
        BufferUsage flags = isDest ? BufferUsage::CopyDst : BufferUsage::CopySrc;
        switch (type) {
        case buffer_type::uniform:
            flags |= BufferUsage::Uniform;
            break;

        case buffer_type::storage:
            flags |= BufferUsage::Storage;
            break;

        case buffer_type::index:
            flags |= BufferUsage::Index;
            break;

        case buffer_type::vertex:
            flags |= BufferUsage::Vertex;
            break;

        case buffer_type::map_read:
            flags |= BufferUsage::MapRead;
            break;

        default:
            ASSERT(false);
        }

        return flags;
    }

    Device m_device;
    size_t m_size;
    BufferUsage m_usage;
    Buffer m_buffer;
    std::atomic<bool> m_done;
    std::function<void(unsigned long, const void*)> m_dataCallback;
    std::shared_ptr<pimpl> m_self_ref;
};
}
