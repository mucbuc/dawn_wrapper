#pragma once

#include "dawn_utils.hpp"

using namespace wgpu;

namespace dawn_wrapper {
struct sampler_wrapper::pimpl {
    pimpl(Device device, sampler_config config)
        : m_sampler(dawn_utils::make_sampler(device, config))
    {
    }

    Sampler m_sampler;
};

}
