#pragma once

#include "dawn_utils.hpp"
#include "dawn_wrapper.h"

#include <dawn/webgpu_cpp.h>

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

    Device m_device;
    CommandEncoder m_encoder;
};
}
