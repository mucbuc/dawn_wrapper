#include "dawn_wrapper.hpp"

#include "dawn_utils.hpp"

using namespace wgpu;

#include "bindgroup_layout_wrapper_impl.hpp"
#include "bindgroup_wrapper_impl.hpp"
#include "buffer_wrapper_impl.hpp"
#include "compute_wrapper_impl.hpp"
#include "encoder_wrapper_impl.hpp"
#include "render_wrapper_impl.hpp"
#include "surface_wrapper_impl.hpp"
#include "texture_output_wrapper_impl.hpp"
#include "texture_wrapper_impl.hpp"

using namespace std;

using namespace dawn_utils;
using namespace literals;

namespace dawn_wrapper {
struct dawn_plugin::dawn_pimpl {
    dawn_pimpl(/*ostream& out*/ const char* label = "")
        : m_device()
        , m_adapter()
        , m_instance(CreateInstance())
        , m_label(label)
        , m_loaded_callback()
    {
    }

    void on_load(std::function<void()> load_callback)
    {
        ASSERT(!m_loaded_callback);

        m_loaded_callback = load_callback;

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
                    pimpl->m_loaded_callback();
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

#ifndef __EMSCRIPTEN__
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
#endif

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

#ifndef __EMSCRIPTEN__
                pimpl->m_device.SetLoggingCallback([](auto type, auto message, auto userdata) {
                    auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                    pimpl->log_error("error requesting webgpu device");
                },
                    pimpl);
#endif

                pimpl->m_loaded_callback();
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

    template <class T>
    void log_error(const char* error, T message)
    {
        cout << error << message.data << endl;
    }

    surface_wrapper make_surface()
    {
        ASSERT(m_device.Get() && m_instance.Get());
        return make_shared<surface_wrapper::pimpl>(m_device, m_instance);
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

    buffer_wrapper make_buffer(size_t size, buffer_type flags, bool isDest)
    {
        return make_shared<buffer_wrapper::pimpl>(m_device, size, flags, isDest);
    }

    texture_wrapper make_texture_1d(size_t size)
    {
        return make_shared<texture_wrapper::pimpl>(m_device, size);
    }

    texture_wrapper make_texture_2d(size_t width, size_t height)
    {
        return make_shared<texture_wrapper::pimpl>(m_device, width, height);
    }

    texture_wrapper make_texture_from_data(vector<uint8_t> data)
    {
        return make_shared<texture_wrapper::pimpl>(m_device, data);
    }

    texture_output_wrapper make_texture_output(size_t width, size_t height)
    {
        return make_shared<texture_output_wrapper::pimpl>(m_device, width, height);
    }

    Device m_device;
    Adapter m_adapter;
    Instance m_instance;
    const string m_label;
    std::function<void()> m_loaded_callback;
};

dawn_plugin::dawn_plugin(/*ostream& o*/)
    : m_pimpl(make_unique<dawn_pimpl>(/*o*/))
{
}

dawn_plugin::~dawn_plugin() = default;

void dawn_plugin::on_load(std::function<void()> load_callback)
{
    m_pimpl->on_load(load_callback);
}

bool dawn_plugin::run()
{
    return m_pimpl->run();
}

surface_wrapper dawn_plugin::make_surface()
{
    return m_pimpl->make_surface();
}

render_wrapper dawn_plugin::make_render()
{
    return m_pimpl->make_render();
}

compute_wrapper dawn_plugin::make_compute()
{
    return m_pimpl->make_compute();
}

buffer_wrapper dawn_plugin::make_dst_buffer(size_t size, buffer_type flags)
{
    return m_pimpl->make_buffer(size, flags, true);
}

buffer_wrapper dawn_plugin::make_src_buffer(size_t size, buffer_type flags)
{
    return m_pimpl->make_buffer(size, flags, false);
}

texture_wrapper dawn_plugin::make_texture_1d(size_t size)
{
    return m_pimpl->make_texture_1d(size);
}

texture_wrapper dawn_plugin::make_texture_2d(size_t width, size_t height)
{
    return m_pimpl->make_texture_2d(width, height);
}

texture_wrapper dawn_plugin::make_texture_from_data(vector<uint8_t> data)
{
    return m_pimpl->make_texture_from_data(data);
}

texture_output_wrapper dawn_plugin::make_texture_output(size_t width, size_t height)
{
    return m_pimpl->make_texture_output(width, height);
}

encoder_wrapper dawn_plugin::make_encoder()
{
    return m_pimpl->make_encoder();
}

bool dawn_plugin::is_valid() const
{
    return m_pimpl && m_pimpl->m_device;
}

}
