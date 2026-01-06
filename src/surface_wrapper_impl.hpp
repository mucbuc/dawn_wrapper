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
        SurfaceDescriptorFromCanvasHTMLSelector canvasDesc {};
        canvasDesc.selector = selector.c_str();

        SurfaceDescriptor surfaceDesc { .nextInChain = &canvasDesc };
        m_surface = m_wgpuInstance.CreateSurface(&surfaceDesc);

        SurfaceConfiguration config {
            .device = m_device,
            .format = TextureFormat::BGRA8Unorm,
            .width = width,
            .height = height,
        };
        m_surface.Configure(&config);
    }

    TextureView getCurrentTextureView()
    {
        SurfaceTexture st;
        m_surface.GetCurrentTexture(&st);

        ASSERT(!st.suboptimal); 
        ASSERT(st.status == SurfaceGetCurrentTextureStatus::Success);

        return st.texture.CreateView();
    }

    void present()
    {
#ifndef __EMSCRIPTEN__
        m_surface.Present();
#endif
    }

private:
    Device m_device;
    Instance m_wgpuInstance;
    ShaderModule m_vertexShader;
    Buffer m_bufferVertex;
    Buffer m_bufferIndex;
    Surface m_surface;
};

}
