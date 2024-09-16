#include "dawn_wrapper.h"

#include <dawn/webgpu_cpp.h>

#include "buffer_wrapper_impl.h"
#include "compute_wrapper_impl.h"
#include "dawn_utils.hpp"
#include "render_wrapper_impl.h"
#include "texture_output_wrapper_impl.h"
#include "texture_wrapper_impl.h"

using namespace std;
using namespace wgpu;
using namespace dawn_utils;
using namespace literals;

namespace dawn_wrapper {
struct dawn_plugin::dawn_pimpl {
    dawn_pimpl(/*ostream& out*/ const char* label = "")
        : m_device()
        , m_adapter()
        , m_instance(CreateInstance())
        , m_label(label)
    {
        request_adapter(m_instance);
    }

    void request_adapter(Instance instance)
    {
        instance.RequestAdapter(
            nullptr,
            [](auto status, auto adapter, auto message, auto userdata) {
                auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                if (status != WGPURequestAdapterStatus_Success) {
                    pimpl->log_error("error requesting webgpu device adapter");
                    return;
                }

                pimpl->m_adapter = Adapter::Acquire(adapter);
                pimpl->request_device(pimpl->m_adapter, pimpl->m_label.c_str());
            },
            this);
    }

    void request_device(Adapter adapter, const char* label = "")
    {

#if 0
        size_t featureCount = adapter.EnumerateFeatures(nullptr);
        vector<FeatureName> supportedFeatures(featureCount);
        adapter.EnumerateFeatures(supportedFeatures.data());
        for (auto f : supportedFeatures) {
            cout << (int) f << endl;
        }
#endif

        DeviceDescriptor deviceDesc = {};
        RequiredLimits requiredLimits = {};
//        requiredLimits.limits.maxStorageBuffersPerShaderStage = 10;
//        requiredLimits.limits.maxSamplersPerShaderStage = 1;
        deviceDesc.requiredLimits = &requiredLimits;
        deviceDesc.deviceLostCallbackInfo.callback = [](auto device, auto reason, auto message, auto userdata) {
            auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
            pimpl->log_error("device lost: ", message);
        };
        deviceDesc.deviceLostCallbackInfo.userdata = this;

        deviceDesc.uncapturedErrorCallbackInfo.callback = [](auto type, auto message, auto userdata) {
            auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
            pimpl->log_error("error: ", message);


            ASSERT(false);
        };
        deviceDesc.uncapturedErrorCallbackInfo.userdata = this;
#if 1
        vector<FeatureName> features = { FeatureName::ShaderF16 };
        deviceDesc.requiredFeatureCount = features.size();
        deviceDesc.requiredFeatures = features.data();
#endif

        deviceDesc.label = label;
        adapter.RequestDevice(
            &deviceDesc, [](auto status, auto device, auto message, auto userdata) {
                auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                if (status != WGPURequestDeviceStatus_Success) {
                    pimpl->log_error("error requesting webgpu device");
                    return;
                }

                pimpl->m_device = Device::Acquire(device);
                pimpl->m_device.SetLoggingCallback([](auto type, auto message, auto userdata) {
                    auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                    pimpl->log_error("error requesting webgpu device");
                },
                    pimpl);
            },
            this);
    }

    bool run()
    {
        m_instance.ProcessEvents();
        return false;
    }

    void log_error(const char* error)
    {
        cout << error << endl;
    }

    void log_error(const char* error, const char* message)
    {
        cout << error << message << endl;
    }

    render_wrapper make_render()
    {
        ASSERT(m_device.Get() && m_instance.Get());
        return make_shared<render_wrapper::pimpl>(m_device, m_instance);
    }

    compute_wrapper make_compute()
    {
        return make_shared<compute_wrapper::pimpl>(m_device);
    }

    encoder_wrapper make_encoder()
    {
        return make_shared<encoder_wrapper::pimpl>(m_device);
    }

    buffer_wrapper make_buffer(unsigned size, BufferType flags, bool isDest)
    {
        return make_shared<buffer_wrapper::pimpl>(m_device, size, flags, isDest);
    }

    buffer_wrapper make_buffer(BufferType flags, bool isDest)
    {
        return make_shared<buffer_wrapper::pimpl>(m_device, flags, isDest);
    }

    texture_wrapper make_texture(unsigned size)
    {
        return make_shared<texture_wrapper::pimpl>(m_device, size);
    }

    texture_wrapper make_texture(unsigned width, unsigned height)
    {
        return make_shared<texture_wrapper::pimpl>(m_device, width, height);
    }

    texture_wrapper make_texture(vector<uint8_t> data)
    {
        return make_shared<texture_wrapper::pimpl>(m_device, data);
    }

    texture_output_wrapper make_texture_output(unsigned width, unsigned height)
    {
        return make_shared<texture_output_wrapper::pimpl>(m_device, width, height);
    }

    Device m_device;
    Adapter m_adapter;
    Instance m_instance;
    const string m_label;
};

dawn_plugin::dawn_plugin(/*ostream& o*/)
    : m_pimpl(make_unique<dawn_pimpl>(/*o*/))
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

buffer_wrapper dawn_plugin::make_buffer(size_t size, BufferType flags, bool isDest)
{
    return m_pimpl->make_buffer(size, flags, isDest);
}

buffer_wrapper dawn_plugin::make_buffer(BufferType flags, bool isDest)
{
    return m_pimpl->make_buffer(flags, isDest);
}

texture_wrapper dawn_plugin::make_texture(size_t size)
{
    return m_pimpl->make_texture(size);
}

texture_wrapper dawn_plugin::make_texture(size_t width, size_t height)
{
    return m_pimpl->make_texture(width, height);
}

texture_wrapper dawn_plugin::make_texture(vector<uint8_t> data)
{
    return m_pimpl->make_texture(data);
}

texture_output_wrapper dawn_plugin::make_texture_output(size_t width, size_t height)
{
    return m_pimpl->make_texture_output(width, height);
}

encoder_wrapper dawn_plugin::make_encoder()
{
    return m_pimpl->make_encoder();
}
}
