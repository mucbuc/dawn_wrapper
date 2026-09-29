#pragma once

#include "bindgroup_layout_wrapper_impl.hpp"
#include "bindgroup_set_impl.hpp"
#include "bindgroup_wrapper_impl.hpp"
#include "dawn_utils.hpp"
#include "encoder_wrapper_impl.hpp"
#include "shader_base.hpp"
#include "surface_wrapper_impl.hpp"

using namespace wgpu;

namespace dawn_wrapper {
struct render_wrapper::pimpl : private shader_base {
    pimpl() = delete;

    pimpl(Device device, Instance wgpuInstance)
        : m_device(device)
        , m_wgpuInstance(wgpuInstance)
        , m_shader()
        , m_vertexShader(dawn_utils::make_shader(m_device,
              R"(@vertex fn vertexMain(@location(0) p: vec2f) -> @builtin(position) vec4f {
    return vec4f(p, 0, 1);
})"))
        , m_pipeline()
        , m_bufferVertex()
        , m_bufferIndex()
        , m_entryPoint()
        , m_surface()
        , m_target()
        , m_config()
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

    void set_surface(surface_wrapper s)
    {
        m_surface = s;
    }

    void set_target(texture_output_wrapper t)
    {
        m_target = t;
    }

    // The target when one is set, else the surface's current texture.
    TextureView getCurrentTextureView()
    {
        if (m_target.is_valid()) {
            return m_target.m_pimpl->get_view();
        }
        return m_surface.m_pimpl->getCurrentTextureView();
    }

    // Presenting is the surface's; an offscreen target has nothing to present.
    void present()
    {
#ifndef __EMSCRIPTEN__
        if (!m_target.is_valid()) {
            m_surface.present();
        }
#endif
    }

    void render(bindgroup_set set, encoder_wrapper encoder)
    {
        auto textureView = getCurrentTextureView();
        ASSERT(textureView);

        auto pass = dawn_utils::begin_render_pass(encoder.m_pimpl->m_encoder, textureView);
        pass.SetPipeline(get_pipeline());

        for (auto entry : set.m_pimpl->m_bindgroups) {
            ASSERT(entry.second.m_pimpl);

            pass.SetBindGroup(entry.first, entry.second.m_pimpl->make_bindgroup(m_device));
        }

        draw(pass);
        pass.End();

        encoder.submit_command_buffer();
        present();
    }

    void render(bindgroup_wrapper bindGroup, encoder_wrapper encoder)
    {
        ASSERT(bindGroup.is_valid());

        auto textureView = getCurrentTextureView();
        ASSERT(textureView);

        auto pass = dawn_utils::begin_render_pass(encoder.m_pimpl->m_encoder, textureView);
        pass.SetPipeline(get_pipeline());

        auto bindGroupImpl = bindGroup.m_pimpl->make_bindgroup(m_device);
        pass.SetBindGroup(0, bindGroupImpl);

        draw(pass);
        pass.End();

        encoder.submit_command_buffer();
        present();
    }

    void render(encoder_wrapper encoder)
    {
        auto pass = dawn_utils::begin_render_pass(encoder.m_pimpl->m_encoder, getCurrentTextureView());
        pass.SetPipeline(get_pipeline());
        draw(pass);
        pass.End();

        encoder.submit_command_buffer();

        present();
    }

    // The draw itself, from the config. With a vertex buffer it is the wrapper's
    // own full-screen triangle, indexed, as it always was; without, the vertex
    // stage builds its own positions and this is a plain instanced draw.
    void draw(RenderPassEncoder& pass)
    {
        if (m_config.vertex_buffer) {
            pass.SetVertexBuffer(0, get_bufferVertex(), 0, get_bufferVertex().GetSize());
            pass.SetIndexBuffer(get_bufferIndex(), IndexFormat::Uint16, 0, get_bufferIndex().GetSize());
            pass.DrawIndexed(m_config.vertex_count, m_config.instance_count, 0, 0, 0);
            return;
        }
        pass.Draw(m_config.vertex_count, m_config.instance_count, 0, 0);
    }

    bindgroup_layout_wrapper make_bindgroup_layout(shader_visibility visibility)
    {
        ShaderStage stage = ShaderStage::Fragment;
        switch (visibility) {
        case shader_visibility::vertex:
            stage = ShaderStage::Vertex;
            break;
        case shader_visibility::fragment:
            stage = ShaderStage::Fragment;
            break;
        case shader_visibility::vertex_and_fragment:
            stage = ShaderStage::Vertex | ShaderStage::Fragment;
            break;
        }
        return std::make_shared<bindgroup_layout_wrapper::pimpl>(stage, m_entryPoint);
    }

    // Applied by the next init_pipeline and by every draw.
    void configure(dawn_wrapper::render_config config)
    {
        m_config = std::move(config);
        if (!m_config.vertex_script.empty()) {
            m_vertexShader = dawn_utils::make_shader(m_device, m_config.vertex_script);
        }
    }

    void compile_shader(std::string script, std::string entryPoint,
        compile_callback on_messages)
    {
        m_shader = dawn_utils::make_shader(m_device, script, entryPoint.c_str());
        m_shader.GetCompilationInfo(CallbackMode::AllowSpontaneous,
            &shader_base::compilation_callback, make_request(std::move(on_messages)));
        m_entryPoint = entryPoint;
    }

    void init_pipeline(bindgroup_layout_wrapper layout)
    {
        ASSERT(m_shader);

        auto bindGroupLayout = layout.m_pimpl->make_bindGroupLayout(m_device, m_entryPoint.c_str());
        m_pipeline = dawn_utils::make_render_pipeline(m_device, { bindGroupLayout },
            m_shader, m_vertexShader, m_entryPoint.c_str(), m_config);
    }

    void init_pipeline(std::initializer_list<bindgroup_layout_wrapper> layouts)
    {
        ASSERT(m_shader);

        std::vector<BindGroupLayout> bgl;
        bgl.reserve(layouts.size());
        for (auto layout : layouts) {
            bgl.push_back(layout.m_pimpl->make_bindGroupLayout(m_device, m_entryPoint.c_str()));
        }
        m_pipeline = dawn_utils::make_render_pipeline(m_device, bgl,
            m_shader, m_vertexShader, m_entryPoint.c_str(), m_config);
    }

    void init_pipeline()
    {
        ASSERT(m_shader);

        m_pipeline = dawn_utils::make_render_pipeline(m_device, {},
            m_shader, m_vertexShader, m_entryPoint.c_str(), m_config);
    }

    RenderPipeline get_pipeline()
    {
        return m_pipeline;
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
    Device m_device;
    Instance m_wgpuInstance;
    ShaderModule m_shader;
    ShaderModule m_vertexShader;
    RenderPipeline m_pipeline;
    Buffer m_bufferVertex;
    Buffer m_bufferIndex;
    std::string m_entryPoint;
    dawn_wrapper::surface_wrapper m_surface;
    dawn_wrapper::texture_output_wrapper m_target;
    // What the pipeline and the draw are built from; defaults are what this
    // wrapper hardcoded before it was configurable.
    dawn_wrapper::render_config m_config;
};

}
