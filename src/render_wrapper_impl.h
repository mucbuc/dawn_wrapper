#pragma once

#include "bindgroup_layout_wrapper_impl.h"
#include "bindgroup_wrapper_impl.h"
#include "buffer_wrapper_impl.h"
#include "dawn_utils.hpp"
#include "dawn_wrapper.h"
#include "encoder_wrapper_impl.h"
#include "texture_wrapper_impl.h"

#include <dawn/webgpu_cpp.h>

namespace dawn_wrapper {
struct render_wrapper::pimpl {

    pimpl() = delete;

    pimpl(Device device, Instance wgpuInstance)
        : m_device(device)
        , m_bindGroupLayout()
        , m_shader()
        , m_vertexShader(dawn_utils::make_shader(m_device,
              R"(@vertex fn vertexMain(@location(0) p: vec2f) -> @builtin(position) vec4f {
    return vec4f(p, 0, 1);
})"))
        , m_pipeline()
        , m_bufferVertex()
        , m_bufferIndex()
        , m_entryPoint()
        , m_wgpuInstance(wgpuInstance)
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

    void setup_surface(GLFWwindow* window, unsigned width, unsigned height, bool opaque)
    {
        using namespace dawn_utils;
        ASSERT(m_wgpuInstance && window);

#ifndef TARGET_HEADLESS
        m_surface = glfw::CreateSurfaceForWindow(m_wgpuInstance, window, opaque);

        SurfaceConfiguration config;
        config.device = m_device;
        config.format = TextureFormat::BGRA8Unorm;
        config.width = width;
        config.height = height;

        m_surface.Configure(&config);
#endif
    }

    TextureView getCurrentTextureView()
    {
        SurfaceTexture st;
        m_surface.GetCurrentTexture(&st);
        return st.texture.CreateView();
    }

    void render(bindgroup_wrapper bindGroup, encoder_wrapper encoder)
    {
        ASSERT(m_bindGroupLayout);

        auto textureView = getCurrentTextureView();
        ASSERT(textureView);

        auto pass = dawn_utils::begin_render_pass(encoder.m_pimpl->m_encoder, textureView);
        pass.SetPipeline(get_pipeline());

        auto bindGroupImpl = bindGroup.m_pimpl->make_bindgroup(m_device, m_bindGroupLayout);
        pass.SetBindGroup(0, bindGroupImpl);

        pass.SetVertexBuffer(0, get_bufferVertex(), 0, get_bufferVertex().GetSize());
        pass.SetIndexBuffer(get_bufferIndex(), IndexFormat::Uint16, 0, get_bufferIndex().GetSize());
        pass.DrawIndexed(3, 1, 0, 0, 0);
        pass.End();

        encoder.submit_command_buffer();

        m_surface.Present();
    }

    void render(encoder_wrapper encoder)
    {
        auto pass = dawn_utils::begin_render_pass(encoder.m_pimpl->m_encoder, getCurrentTextureView());
        pass.SetPipeline(get_pipeline());
        pass.SetVertexBuffer(0, get_bufferVertex(), 0, get_bufferVertex().GetSize());
        pass.SetIndexBuffer(get_bufferIndex(), IndexFormat::Uint16, 0, get_bufferIndex().GetSize());
        pass.DrawIndexed(3, 1, 0, 0, 0);
        pass.End();

        encoder.submit_command_buffer();

        m_surface.Present();
    }

    bindgroup_layout_wrapper make_bindgroup_layout()
    {
        return std::make_shared<bindgroup_layout_wrapper::pimpl>(ShaderStage::Fragment);
    }

    bindgroup_wrapper make_bindgroup()
    {
        return std::make_shared<bindgroup_wrapper::pimpl>();
    }

    void make_fragmentShader(std::string script, std::string entryPoint)
    {
        m_shader = dawn_utils::make_shader(m_device, script);
        m_shader.GetCompilationInfo(&compilation_callback, this);
        m_entryPoint = entryPoint;
    }

    void make_pipeline(bindgroup_layout_wrapper layout)
    {
        ASSERT(m_shader);

        m_bindGroupLayout = dawn_utils::make_bindGroupLayout(m_device, layout.m_pimpl->m_layoutEntries);
        m_pipeline = dawn_utils::make_render_pipeline(m_device, m_bindGroupLayout, m_shader, m_vertexShader, m_entryPoint.c_str());
    }

    void make_pipeline()
    {
        ASSERT(m_shader);

        m_pipeline = dawn_utils::make_render_pipeline(m_device, m_shader, m_vertexShader, m_entryPoint.c_str());
    }

    RenderPipeline get_pipeline()
    {
        return m_pipeline;
    }

    BindGroupLayout get_bindGroupLayout()
    {
        return m_bindGroupLayout;
    }

    Buffer get_bufferVertex()
    {
        return m_bufferVertex;
    }

    Buffer get_bufferIndex()
    {
        return m_bufferIndex;
    }

private:
    static void compilation_callback(WGPUCompilationInfoRequestStatus status, struct WGPUCompilationInfo const* compilationInfo, void* userdata)
    {
        pimpl* instance(reinterpret_cast<pimpl*>(userdata));
        std::stringstream messages;
        size_t errorCount = 0;
        for (auto i = 0; i < compilationInfo->messageCount; ++i) {
            const auto message = compilationInfo->messages[i];
            if (message.type == WGPUCompilationMessageType_Error) {
                messages << "Error(" << i << "): ";
                ++errorCount;
            } else if (message.type == WGPUCompilationMessageType_Warning) {
                messages << "Warning(" << i << "): ";
            } else if (message.type == WGPUCompilationMessageType_Info) {
                messages << "Info(" << i << "): ";
            }

            messages << message.message << std::endl;
        }

        std::cout << messages.str() << std::endl;
        //      instance->m_shaderCompileCallback(messages.str());

        //        if (!errorCount) {
        //            instance->setFragmentShaderUser();
        //        }
    }

    Device m_device;
    BindGroupLayout m_bindGroupLayout;
    ShaderModule m_shader;
    ShaderModule m_vertexShader;
    RenderPipeline m_pipeline;
    Buffer m_bufferVertex;
    Buffer m_bufferIndex;
    std::string m_entryPoint;
    Surface m_surface;
    Instance m_wgpuInstance;
};

}
