#include "dawn_wrapper.h"

#include <dawn/webgpu_cpp.h>

#include "buffer_wrapper_impl.h"
#include "compute_wrapper_impl.h"
#include "dawn_utils.hpp"
#include "render_wrapper_impl.h"
#include "texture_wrapper_impl.h"

using namespace std;
using namespace wgpu;
using namespace dawn_utils;
using namespace std::literals;

namespace dawn_wrapper {
struct dawn_plugin::dawn_pimpl {
    dawn_pimpl(/*std::ostream& out*/)
    {
        m_instance = CreateInstance();
        request_adapter(m_instance);
    }

    void request_adapter(Instance instance)
    {
        instance.RequestAdapter(
            nullptr,
            [](WGPURequestAdapterStatus status, WGPUAdapter adapter, const char* message, void* userdata) {
                auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                if (status != WGPURequestAdapterStatus_Success) {
                    pimpl->log_error("error requesting webgpu device adapter");
                    return;
                }

                pimpl->m_adapter = Adapter::Acquire(adapter);
                pimpl->request_device(pimpl->m_adapter);
            },
            this);
    }

    void request_device(Adapter adapter)
    {
        DeviceDescriptor deviceDesc = {};
        RequiredLimits requiredLimits = {};
        requiredLimits.limits.maxStorageBuffersPerShaderStage = 2;
        requiredLimits.limits.maxSamplersPerShaderStage = 1;
        deviceDesc.requiredLimits = &requiredLimits;

        adapter.RequestDevice(
            &deviceDesc, [](auto status, auto device, const char* message, void* userdata) {
                auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                if (status != WGPURequestDeviceStatus_Success) {
                    pimpl->log_error("error requesting webgpu device");
                    return;
                }

                pimpl->m_device = Device::Acquire(device);
                pimpl->m_device.SetUncapturedErrorCallback([](WGPUErrorType type, char const* message, void* userdata) {
                    auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                    pimpl->log_error("error: ", message);
                },
                    pimpl);

                pimpl->m_device.SetDeviceLostCallback([](auto reason, auto message, auto userdata) {
                    auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                    pimpl->log_error("device lost: ", message);
                },
                    pimpl);
            },
            this);
    }

    bool run()
    {
        m_instance.ProcessEvents();
    }

    void log_error(const char* error)
    {
        std::cout << error << std::endl;
    }

    void log_error(const char* error, const char* message)
    {
        std::cout << error << message << std::endl;
    }

    render_wrapper make_render()
    {
        ASSERT(m_device.Get() && m_instance.Get());
        return std::make_shared<render_wrapper::pimpl>(m_device, m_instance);
    }

    compute_wrapper make_compute()
    {
        return std::make_shared<compute_wrapper::pimpl>(m_device);
    }

    encoder_wrapper make_encoder()
    {
        return std::make_shared<encoder_wrapper::pimpl>(m_device);
    }

    buffer_wrapper make_buffer(unsigned size, BufferType flags, bool isDest)
    {
        return std::make_shared<buffer_wrapper::pimpl>(m_device, size, flags, isDest);
    }

    buffer_wrapper make_buffer(BufferType flags, bool isDest)
    {
        return std::make_shared<buffer_wrapper::pimpl>(m_device, flags, isDest);
    }

    texture_wrapper make_texture(unsigned size)
    {
        return std::make_shared<texture_wrapper::pimpl>(m_device, size);
    }

    texture_wrapper make_texture(std::vector<uint8_t> data)
    {
        return std::make_shared<texture_wrapper::pimpl>(m_device, data);
    }

    Device m_device;
    Adapter m_adapter;
    Instance m_instance;
};

dawn_plugin::dawn_plugin(/*std::ostream& o*/)
    : m_pimpl(std::make_unique<dawn_pimpl>(/*o*/))
{
}

dawn_plugin::~dawn_plugin() = default;

bool dawn_plugin::run()
{
    return m_pimpl->run();
}

render_wrapper dawn_plugin::make_render()
{
    return m_pimpl->make_render();
}

compute_wrapper dawn_plugin::make_compute()
{
    return m_pimpl->make_compute();
}

buffer_wrapper dawn_plugin::make_buffer(unsigned size, BufferType flags, bool isDest)
{
    return m_pimpl->make_buffer(size, flags, isDest);
}

buffer_wrapper dawn_plugin::make_buffer(BufferType flags, bool isDest)
{
    return m_pimpl->make_buffer(flags, isDest);
}

texture_wrapper dawn_plugin::make_texture(unsigned size)
{
    return m_pimpl->make_texture(size);
}

texture_wrapper dawn_plugin::make_texture(std::vector<uint8_t> data)
{
    return m_pimpl->make_texture(data);
}

encoder_wrapper dawn_plugin::make_encoder()
{
    return m_pimpl->make_encoder();
}
}
