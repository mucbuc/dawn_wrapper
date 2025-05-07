#pragma once

#include "bindgroup_layout_wrapper_impl.hpp"
#include "bindgroup_wrapper_impl.hpp"
#include "buffer_wrapper_impl.hpp"
#include "dawn_utils.hpp"
#include "dawn_wrapper.hpp"
#include "encoder_wrapper_impl.hpp"
#include "shader_base.hpp"

using namespace wgpu;

namespace dawn_wrapper {
struct compute_wrapper::pimpl : private shader_base {
    pimpl() = delete;

    pimpl(Device device)
        : m_device(device)
        , m_bindGroupLayout()
        , m_shader()
        , m_pipeline()
        , m_entryPoint()
    {
    }

    void compile_shader(std::string script, std::string entryPoint)
    {
        m_shader = dawn_utils::make_compute_shader(m_device, script, entryPoint.c_str());
        m_shader.GetCompilationInfo(&shader_base::compilation_callback, this);
        m_entryPoint = entryPoint;
    }

    void init_pipeline(bindgroup_layout_wrapper layout)
    {
        m_bindGroupLayout = dawn_utils::make_bindGroupLayout(m_device, layout.m_pimpl->m_layoutEntries, m_entryPoint.c_str());
        m_pipeline = dawn_utils::make_compute_pipeline(m_device, m_shader, m_bindGroupLayout, m_entryPoint.c_str());
    }

    bool compute(bindgroup_wrapper bindGroup, unsigned width, unsigned height, encoder_wrapper encoder)
    {
        ASSERT(get_pipeline());
        ASSERT(width < 65535);
        ASSERT(height < 65535);

        auto computePass = dawn_utils::begin_compute_pass(encoder.m_pimpl->m_encoder);
        computePass.SetPipeline(get_pipeline());
        computePass.SetBindGroup(0, bindGroup.m_pimpl->make_bindgroup(m_device, m_bindGroupLayout), 0, nullptr);
        computePass.DispatchWorkgroups(width, height, 1);
        computePass.End();

        return false;
    }

    ComputePipeline get_pipeline()
    {
        return m_pipeline;
    }

    BindGroupLayout get_bindGroupLayout()
    {
        return m_bindGroupLayout;
    }

    bindgroup_layout_wrapper make_bindgroup_layout()
    {
        return std::make_shared<bindgroup_layout_wrapper::pimpl>(ShaderStage::Compute);
    }

    bindgroup_wrapper make_bindgroup()
    {
        return std::make_shared<bindgroup_wrapper::pimpl>(m_entryPoint);
    }

    Device m_device;
    BindGroupLayout m_bindGroupLayout;
    ShaderModule m_shader;
    ComputePipeline m_pipeline;
    std::string m_entryPoint;
};

}
