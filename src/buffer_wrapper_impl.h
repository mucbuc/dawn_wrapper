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
        , m_done(true)
    {
    }

    pimpl(Device device, BufferType flags, bool isDest)
        : m_device(device)
        , m_isDest(isDest)
        , m_usage(getBufferUsageFromType(flags, m_isDest))
        , m_buffer()
        , m_done(true)
    {
    }

    void write(void* data, size_t size)
    {
        if (!m_buffer || m_buffer.GetSize() < size) {
            m_buffer = dawn_utils::make_buffer(m_device, size, m_usage);
        }

        if (m_isDest) {
            m_device.GetQueue().WriteBuffer(m_buffer, 0, data, size);
        }
    }

    void write(const std::vector<uint8_t>& colors)
    {
        write((void*)colors.data(), colors.size());
    }

    static void callback(auto status, auto userData)
    {
        if (status == WGPUBufferMapAsyncStatus_Success) {
            pimpl* instance = (pimpl*)userData;
            const auto size = instance->m_buffer.GetSize();
            const int8_t* output = (const int8_t*)instance->m_buffer.GetConstMappedRange(0, size);
            for (auto i = 0; i < size; ++i) {
                std::cout << unsigned(output[i]) << ", ";
            }
            std::cout << std::endl;
            instance->m_buffer.Unmap();
            instance->m_done = true;
        }
    }

    static void callback2(auto status, auto userData)
    {
        pimpl* instance = (pimpl*)userData;
        if (status == WGPUBufferMapAsyncStatus_Success) {
            const auto size = instance->m_buffer.GetSize();
            instance->m_dataCallback(size, instance->m_buffer.GetConstMappedRange(0, size));
        
            instance->m_buffer.Unmap();
            instance->m_done = true;
        }
        else
        {
            instance->m_dataCallback(0, nullptr);
        }
    }

    void print_output()
    {
        auto buffer_size = m_buffer.GetSize();
        m_done = false;
        m_buffer.MapAsync(MapMode::Read, 0, buffer_size, &callback, this);
    }
    
    void get_output(std::function<void(unsigned, const void*)> cb)
    {
        m_dataCallback = cb;
        
        auto buffer_size = m_buffer.GetSize();
        m_done = false;
        m_buffer.MapAsync(MapMode::Read, 0, buffer_size, &callback2, this);
    }

    bool done()
    {
        return m_done;
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

        case BufferType::MapRead:
            flags |= BufferUsage::MapRead;
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
    std::atomic<bool> m_done;
    std::function<void(unsigned, const void*)> m_dataCallback;
};
}
