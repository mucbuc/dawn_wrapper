#pragma once

#include "bindgroup_layout_wrapper_impl.h"
#include "bindgroup_wrapper_impl.h"
#include "buffer_wrapper_impl.h"
#include "dawn_utils.hpp"
#include "dawn_wrapper.h"
#include "encoder_wrapper_impl.h"

#include <dawn/webgpu_cpp.h>

namespace dawn_wrapper {
struct compute_wrapper::pimpl {
    pimpl() = delete;

    pimpl(Device device)
        : m_device(device)
        , m_bindGroupLayout()
        , m_shader()
        , m_pipeline()
        , m_entryPoint()
    {
    }

    void make_shader(std::string script, std::string entryPoint)
    {
        m_shader = dawn_utils::make_compute_shader(m_device, script);
        m_entryPoint = entryPoint;
    }

    void make_pipeline(bindgroup_layout_wrapper layout)
    {
        m_bindGroupLayout = dawn_utils::make_bindGroupLayout(m_device, layout.m_pimpl->m_layoutEntries);
        m_pipeline = dawn_utils::make_compute_pipeline(m_device, m_shader, m_bindGroupLayout, m_entryPoint.c_str());
    }

    bool compute(bindgroup_wrapper bindGroup, unsigned width, unsigned height, encoder_wrapper encoder)
    {
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
        return std::make_shared<bindgroup_wrapper::pimpl>();
    }

private:
    Device m_device;
    BindGroupLayout m_bindGroupLayout;
    ShaderModule m_shader;
    ComputePipeline m_pipeline;
    std::string m_entryPoint;
};

}
