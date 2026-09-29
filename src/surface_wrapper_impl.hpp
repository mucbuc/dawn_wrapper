#pragma once

#include "dawn_utils.hpp"
#include "dawn_wrapper.hpp"

using namespace wgpu;

namespace dawn_wrapper {
struct surface_wrapper::pimpl {
    pimpl() = delete;

    pimpl(Device device, Instance wgpuInstance)
        : m_device(device)
        , m_wgpuInstance(wgpuInstance)
        , m_vertexShader(dawn_utils::make_shader(m_device,
              R"(@vertex fn vertexMain(@location(0) p: vec2f) -> @builtin(position) vec4f {
    return vec4f(p, 0, 1);
})"))
        , m_bufferVertex()
        , m_bufferIndex()
        , m_surface()
    {
        std::vector<float> verts { -1, 3, -1, -1, 3, -1 };
        const auto vertBytes = verts.size() * sizeof(float);

        m_bufferVertex = dawn_utils::make_buffer(m_device, vertBytes, BufferUsage::Vertex | BufferUsage::CopyDst);
        m_device.GetQueue().WriteBuffer(m_bufferVertex, 0, verts.data(), vertBytes);

        std::vector<uint16_t> indecies { 0, 1, 2 };
        const auto indexBytes = verts.size() * sizeof(uint16_t);

        m_bufferIndex = dawn_utils::make_buffer(m_device, indexBytes, BufferUsage::Index | BufferUsage::CopyDst);
        m_device.GetQueue().WriteBuffer(m_bufferIndex, 0, indecies.data(), indexBytes);
    }

    void setup(GLFWwindow* window, unsigned width, unsigned height, bool opaque)
    {
        using namespace dawn_utils;
        ASSERT(m_wgpuInstance && window);

#ifndef __EMSCRIPTEN__
        m_surface = glfw::CreateSurfaceForWindow(m_wgpuInstance, window); //, opaque);
#endif

        SurfaceConfiguration config;
        config.device = m_device;
        config.format = TextureFormat::BGRA8Unorm;
        config.width = width;
        config.height = height;

        m_surface.Configure(&config);
    }

    void setup(std::string selector, unsigned width, unsigned height)
    {
        EmscriptenSurfaceSourceCanvasHTMLSelector canvasDesc {};
        canvasDesc.selector = selector.c_str();

        SurfaceDescriptor surfaceDesc { .nextInChain = &canvasDesc };
        m_surface = m_wgpuInstance.CreateSurface(&surfaceDesc);

        // Premultiplied rather than the default Auto, which resolves to opaque
        // for a canvas. This alone changes nothing: a fragment that returns
        // alpha 1 composites identically either way. It is what lets a fragment
        // return anything else.
        SurfaceConfiguration config {
            .device = m_device,
            .format = TextureFormat::BGRA8Unorm,
            .width = width,
            .height = height,
            .alphaMode = CompositeAlphaMode::Premultiplied,
        };
        m_surface.Configure(&config);
    }

    // A null view means "no frame this time", and callers must check it.
    //
    // This was ASSERT(status == SuccessOptimal), which was wrong twice over.
    // SUBOPTIMAL IS A SUCCESS — it is what a surface returns after a resize,
    // when the swapchain still works but no longer matches the window — so the
    // assert treated a normal event as fatal. And on the failures that are real
    // (Lost, Outdated, Timeout, Error) st.texture is null, so compiling the
    // assert out under NDEBUG would have called CreateView() on nothing.
    //
    // That second half is why this had to change before NDEBUG could be turned
    // on at all: the assert was the only thing standing between a lost device
    // and undefined behaviour, and asserts are precisely what a release build
    // removes.
    TextureView getCurrentTextureView()
    {
        SurfaceTexture st;
        m_surface.GetCurrentTexture(&st);

        if (st.status == SurfaceGetCurrentTextureStatus::SuccessOptimal
            || st.status == SurfaceGetCurrentTextureStatus::SuccessSuboptimal) {
            return st.texture.CreateView();
        }

        // Once per surface, not once per frame. A surface that has gone gives
        // the same answer sixty times a second, and a log that repeats at that
        // rate buries the first occurrence, which is the informative one.
        if (!m_reported_bad_status) {
            m_reported_bad_status = true;
            std::cerr << "[dawn_wrapper] surface has no drawable texture: "
                      << status_name(st.status)
                      << ". Frames are being skipped." << std::endl;
        }
        return {};
    }

    void present()
    {
#ifndef __EMSCRIPTEN__
        m_surface.Present();
#endif
    }

    std::pair<unsigned, unsigned> get_width_and_height() const
    {
        SurfaceTexture surface_texture;
        m_surface.GetCurrentTexture(&surface_texture);
        return { surface_texture.texture.GetWidth(), surface_texture.texture.GetHeight() };
    }

private:
    static const char* status_name(SurfaceGetCurrentTextureStatus s)
    {
        switch (s) {
        case SurfaceGetCurrentTextureStatus::SuccessOptimal: return "optimal";
        case SurfaceGetCurrentTextureStatus::SuccessSuboptimal: return "suboptimal";
        case SurfaceGetCurrentTextureStatus::Timeout: return "timeout";
        case SurfaceGetCurrentTextureStatus::Outdated: return "outdated";
        case SurfaceGetCurrentTextureStatus::Lost: return "lost";
        case SurfaceGetCurrentTextureStatus::Error: return "error";
        }
        return "unknown";
    }

    bool m_reported_bad_status = false;
    Device m_device;
    Instance m_wgpuInstance;
    ShaderModule m_vertexShader;
    Buffer m_bufferVertex;
    Buffer m_bufferIndex;
    Surface m_surface;
};

}
