#include "dawn_wrapper.hpp"

#include "dawn_utils.hpp"

// Readable names for FeatureName, for the adapter report below. Without it a
// feature list prints as bare integers, which is a diagnostic only for someone
// holding the header open.
#include <dawn/webgpu_cpp_print.h>
#include <sstream>

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

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

// The device callbacks below need a way out to the page. There is no return path
// from a spontaneous callback — nobody is awaiting it — so it calls a hook on
// Module if the embed installed one, and does nothing if it did not.
//
// StringView is not necessarily null-terminated: a length of SIZE_MAX is the
// sentinel meaning "it is". Both cases have to be handled or this reads past the
// end of a message exactly when something has already gone wrong.
static void notify_page(const char* hook, wgpu::StringView message)
{
    std::string text;
    if (message.data != nullptr) {
        text = (message.length == SIZE_MAX)
            ? std::string(message.data)
            : std::string(message.data, message.length);
    }
    EM_ASM({
        var fn = Module[UTF8ToString($0)];
        if (typeof fn === "function") fn(UTF8ToString($1));
    },
        hook, text.c_str());
}
#endif

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

    void on_load(std::function<void(std::string error)> load_callback)
    {
        ASSERT(!m_loaded_callback);

        m_loaded_callback = load_callback;

        request_adapter(m_instance);
    }

    void request_adapter(Instance instance)
    {
        instance.RequestAdapter(
            nullptr,
            CallbackMode::AllowSpontaneous,
            [](RequestAdapterStatus status, Adapter adapter, const char* message, void* userdata) {
                auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                if (status != RequestAdapterStatus::Success) {
                    std::string error = "requesting a WebGPU adapter failed";
                    if (message && *message) {
                        error += std::string(": ") + message;
                    }
                    pimpl->log_error(error.c_str());
                    pimpl->m_loaded_callback(error);
                    return;
                }

                pimpl->m_adapter = adapter;
                pimpl->request_device(pimpl->m_adapter, pimpl->m_label.c_str());
            },
            (void*)this);
    }

    void request_device(Adapter adapter, const char* label = "")
    {

        DeviceDescriptor deviceDesc = {};

        // ASK FOR WHAT THE ADAPTER HAS, CLAMPED TO WHAT IT REPORTS.
        //
        // This used to be a zero-initialised Limits, which asks for nothing and
        // therefore gets WebGPU's guaranteed floor: 128 MiB of storage buffer
        // binding and a 256 MiB buffer. Those are the numbers every conforming
        // implementation must offer, not the numbers a machine actually has —
        // the Apple adapter this was written on reports 4096 MiB for both, 32x
        // and 16x the floor. The glyph store was sized against the floor and had
        // been refusing glyphs with thirty-two times the room unasked for.
        //
        // CLAMPED, and that word is the whole safety argument. A required limit
        // the adapter cannot meet makes requestDevice FAIL, and a device that
        // fails to come up is a blank canvas with no type on it. Copying the
        // adapter's own reported values back into the request cannot be
        // unsatisfiable, so this raises the ceiling on machines that have the
        // room and changes nothing at all on machines that do not.
        //
        // It is deliberately NOT a feature request. Requiring something unused
        // is how npm 0.0.1 shipped: ShaderF16 was required, never referenced by
        // any shader, and broke device creation on every machine without it.
        // Limits are the safe half of that surface precisely because they can be
        // clamped to what is on offer; features cannot.
        Limits supported = {};
        const bool have_limits = (adapter.GetLimits(&supported) == Status::Success);

        Limits requiredLimits = {};
        if (have_limits) {
            requiredLimits.maxBufferSize = supported.maxBufferSize;
            requiredLimits.maxStorageBufferBindingSize
                = supported.maxStorageBufferBindingSize;
            m_max_storage_buffer_size = supported.maxStorageBufferBindingSize;
        }
        deviceDesc.requiredLimits = &requiredLimits;

        // Both of these used to be native-only, which meant the build that
        // actually ships had NO device-lost handling at all — a GPU reset, a
        // laptop switching graphics, or a mobile tab being evicted just stopped
        // the frames with nothing said anywhere. The native build cannot even be
        // compiled from this repo (the root CMakeLists refuses a non-Emscripten
        // toolchain), so this handler had never once run.
        deviceDesc.SetDeviceLostCallback(
            CallbackMode::AllowSpontaneous, [](const wgpu::Device& device, wgpu::DeviceLostReason reason, wgpu::StringView message, void* userdata) {
                auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                // THE REASON, which this callback has always received and always
                // thrown away. It is the difference between four unrelated
                // events wearing one sentence:
                //
                //   Destroyed         someone called destroy, or the page is
                //                     tearing down — usually not a fault at all
                //   Unknown           a genuine loss: driver reset, GPU removed,
                //                     a tab evicted on mobile
                //   CallbackCancelled the instance went away with futures still
                //                     pending; the device is incidental
                //   FailedCreation    it never came up
                //
                // Chasing a CI failure spent several runs unable to separate the
                // first from the second, because the message alone cannot: the
                // browser's own device.lost text is forwarded verbatim by the
                // emdawnwebgpu glue, so "Device was destroyed." can arrive from
                // Chrome rather than from Dawn's internal constant of the same
                // wording. This matters in production too — embed.js shows a
                // fault for any of these, and a page evicted on mobile is not a
                // bug the way a driver reset is.
                const char* why = "unknown";
                switch (reason) {
                case wgpu::DeviceLostReason::Destroyed: why = "destroyed"; break;
                case wgpu::DeviceLostReason::CallbackCancelled:
                    why = "callback-cancelled";
                    break;
                case wgpu::DeviceLostReason::FailedCreation:
                    why = "failed-creation";
                    break;
                default: break;
                }
                pimpl->log_error("device lost: ", message);
#ifdef __EMSCRIPTEN__
                std::string text = std::string("[") + why + "] ";
                if (message.data != nullptr) {
                    text += (message.length == SIZE_MAX)
                        ? std::string(message.data)
                        : std::string(message.data, message.length);
                }
                notify_page("onDeviceLost", text.c_str());
#endif
            },
            (void*)this);

        deviceDesc.SetUncapturedErrorCallback([](const wgpu::Device& device, wgpu::ErrorType type, wgpu::StringView message, void* userdata) {
            auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
            pimpl->log_error("error: ", message);
#ifdef __EMSCRIPTEN__
            // Deliberately NOT the assert the native path takes. An uncaptured
            // error is often a recoverable validation complaint, and aborting the
            // runtime over one takes down a page that driftype is a guest on.
            // Report it and let the embed decide.
            notify_page("onUncapturedError", message);
#else
            ASSERT(false);
#endif
        },
            (void*)this);

        // NO required features, and ShaderF16 in particular is not one.
        //
        // It was required here behind an `#if 1` and never used: no shader in
        // driftype, fieldfactory or dr_dawn declares `enable f16;` or names the
        // type — not src/wgsl/, not presets/, not the WGSL the factory composes
        // at runtime. shader-f16 is an OPTIONAL WebGPU feature, and requiring an
        // unsupported one does not degrade, it fails RequestDevice outright.
        //
        // So every adapter without f16 — plenty of integrated and mobile
        // hardware, and every software implementation — granted an adapter, let
        // the page download 24MB, and then died at device creation, for a
        // capability nothing asked of it. Found by driftype's CI on 2026-09-22,
        // where SwiftShader reported exactly that: "error requesting webgpu
        // device", then "device lost: Device creation failed." dr_dawn's CI hit
        // it again on 2026-09-29.
        //
        // If a shader ever does need f16, this comes back as a feature that is
        // requested WHEN THE ADAPTER HAS IT, and the editor's pre-flight check
        // learns to ask for it before fetching the engine — not as an
        // unconditional requirement.

        deviceDesc.label = label;
        adapter.RequestDevice(
            &deviceDesc, CallbackMode::AllowSpontaneous, [](RequestDeviceStatus status, Device device, const char* message, void* userdata) {
                auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                // Reported to the caller as well as logged: a device that never
                // arrives otherwise leaves on_load's callback uncalled, and the
                // caller waits on nothing.
                if (status != RequestDeviceStatus::Success) {
                    // `message` was being discarded, and it is the reason. Losing
                    // it is why the f16 failure took a browser console and a CI
                    // log to explain on 2026-09-22.
                    std::string error = "requesting a WebGPU device failed";
                    if (message && *message) {
                        error += std::string(": ") + message;
                    }
                    pimpl->log_error(error.c_str());
                    pimpl->log_adapter_features();
                    pimpl->m_loaded_callback(error);
                    return;
                }

                pimpl->m_device = device;

#ifndef __EMSCRIPTEN__

                // Was reporting "error requesting webgpu device" — copy-paste from
                // the branch above, in a callback that fires for any device log
                // long after the request succeeded. Native-only, so it has never
                // run in a shipped build.
                pimpl->m_device.SetLoggingCallback([](LoggingType type, const char* message, void* userdata) {
                    auto pimpl = reinterpret_cast<dawn_pimpl*>(userdata);
                    pimpl->log_error("device log: ", message ? message : "(no message)");
                },
                    (void*)pimpl);
#endif

                pimpl->m_loaded_callback("");
            },
            (void*)this);
    }

    bool run()
    {
        m_instance.ProcessEvents();
        return false;
    }

    void on_work_done(std::function<void(std::string error)> cb)
    {
        if (!m_device) {
            cb("no device");
            return;
        }
        // AllowSpontaneous, like every other callback here: nobody is pumping a
        // wait loop, and the page's event loop is what makes progress.
        m_device.GetQueue().OnSubmittedWorkDone(CallbackMode::AllowSpontaneous,
            [cb](QueueWorkDoneStatus status, StringView message) {
                if (status == QueueWorkDoneStatus::Success) {
                    cb("");
                    return;
                }
                std::string text = (status == QueueWorkDoneStatus::CallbackCancelled)
                    ? "queue work cancelled"
                    : "queue work failed";
                if (message.data != nullptr) {
                    text += ": ";
                    text += (message.length == SIZE_MAX)
                        ? std::string(message.data)
                        : std::string(message.data, message.length);
                }
                cb(text);
            });
    }

    void log_error(const char* error)
    {
        cout << error << endl;
    }

    // What the adapter actually offered, printed only once device creation has
    // already failed.
    //
    // This existed as dead code behind an `#if 0`, five lines above a
    // RequestDevice that required ShaderF16 — the diagnostic for the bug, sitting
    // switched off next to the bug. It had also rotted: EnumerateFeatures is gone
    // from the current header, so enabling it as written would not have compiled.
    //
    // On the failure path only. A device that came up needs no feature dump, and
    // printing one on every boot is noise in somebody else's console.
    void log_adapter_features()
    {
        if (!m_adapter) {
            log_error("no adapter to report features for");
            return;
        }

        SupportedFeatures supported;
        m_adapter.GetFeatures(&supported);

        ostringstream out;
        out << "adapter offered " << supported.featureCount << " feature(s):";
        for (size_t i = 0; i < supported.featureCount; ++i) {
            out << " " << supported.features[i];
        }
        log_error(out.str().c_str());
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

    void push_error_scope()
    {
        ASSERT(m_device.Get());
        m_device.PushErrorScope(ErrorFilter::Validation);
    }

    void pop_error_scope(std::function<void(std::string)> on_error)
    {
        ASSERT(m_device.Get());
        m_device.PopErrorScope(CallbackMode::AllowSpontaneous,
            [on_error = std::move(on_error)](PopErrorScopeStatus status, ErrorType type, StringView message) {
                std::string text;
                if (status != PopErrorScopeStatus::Success) {
                    text = "the error scope did not resolve";
                } else if (type != ErrorType::NoError) {
                    text = std::string(message.data ? std::string_view(message) : std::string_view("unknown error"));
                }
                if (on_error) {
                    on_error(text);
                }
            });
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
    // Filled from the adapter at device creation; 0 until then. Not the spec
    // floor — see the clamped request in request_device.
    size_t m_max_storage_buffer_size = 0;
    std::function<void(std::string error)> m_loaded_callback;
};

dawn_plugin::dawn_plugin(/*ostream& o*/)
    : m_pimpl(make_unique<dawn_pimpl>(/*o*/))
{
}

dawn_plugin::~dawn_plugin() = default;

void dawn_plugin::on_load(std::function<void(std::string error)> load_callback)
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

size_t dawn_plugin::max_storage_buffer_size() const
{
    return m_pimpl->m_max_storage_buffer_size;
}

void dawn_plugin::on_work_done(std::function<void(std::string error)> cb)
{
    m_pimpl->on_work_done(cb);
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

void dawn_plugin::push_error_scope()
{
    m_pimpl->push_error_scope();
}

void dawn_plugin::pop_error_scope(std::function<void(std::string)> on_error)
{
    m_pimpl->pop_error_scope(std::move(on_error));
}

bool dawn_plugin::is_valid() const
{
    return m_pimpl && m_pimpl->m_device;
}

}
