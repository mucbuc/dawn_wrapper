#pragma once

#include <cassert>
#include <chrono>
#include <ctime>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include <webgpu/webgpu_cpp.h>
#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

#include "dawn_wrapper.hpp"

#ifndef ASSERT
#define ASSERT(p) assert((p))
#endif

using namespace wgpu;

namespace dawn_utils {

static void write_texture(Device& device, Texture& texture, TextureDescriptor& desc, std::vector<uint8_t> colorTexture)
{
    TexelCopyTextureInfo destination {};
    destination.texture = texture;
    destination.mipLevel = 0;
    destination.origin = { 0, 0, 0 };
    destination.aspect = TextureAspect::All;

    TexelCopyBufferLayout source {};
    source.offset = 0;
    source.bytesPerRow = 4 * desc.size.width;
    source.rowsPerImage = desc.size.height;

    device.GetQueue().WriteTexture(&destination, colorTexture.data(), colorTexture.size(), &source, &desc.size);
}

static BindGroup make_bindGroup(Device& device, BindGroupLayout& layout, const std::vector<BindGroupEntry>& entries, const char* label = "")
{
    BindGroupDescriptor bindGroupDesc;
    bindGroupDesc.entryCount = entries.size();
    bindGroupDesc.entries = entries.data();
    bindGroupDesc.layout = layout;
    bindGroupDesc.label = label;
    return device.CreateBindGroup(&bindGroupDesc);
}

template <class... T>
static BindGroup make_bindGroup(Device& device, BindGroupLayout& layout, T... entriesIntializer)
{
    std::vector<BindGroupEntry> entries { entriesIntializer... };
    return make_bindGroup(device, layout, entries);
}

static BindGroupLayout make_bindGroupLayout(Device& device, const std::vector<BindGroupLayoutEntry>& entries, const char* label = "")
{
    ASSERT(!entries.empty());

    BindGroupLayoutDescriptor bindGroupLayoutDesc = {};
    bindGroupLayoutDesc.entryCount = entries.size();
    bindGroupLayoutDesc.entries = entries.data();
    bindGroupLayoutDesc.label = label;
    return device.CreateBindGroupLayout(&bindGroupLayoutDesc);
}

template <class... T>
static BindGroupLayout make_bindGroupLayout(Device& device, T... entriesIntializer)
{
    std::vector<BindGroupLayoutEntry> entries { entriesIntializer... };
    return make_bindGroupLayout(device, entries);
}

static SamplerDescriptor make_samplerDesc(AddressMode mode, const char* label) // = AddressMode::Repeat)
{
    SamplerDescriptor samplerDesc = {};
    samplerDesc.addressModeU = mode;
    samplerDesc.addressModeV = mode;
    samplerDesc.addressModeW = mode;
    samplerDesc.magFilter = FilterMode::Linear;
    samplerDesc.minFilter = FilterMode::Linear;
    samplerDesc.mipmapFilter = MipmapFilterMode::Linear;
    samplerDesc.lodMinClamp = 0.0f;
    samplerDesc.lodMaxClamp = 1.0f;
    samplerDesc.compare = CompareFunction::Undefined;
    samplerDesc.maxAnisotropy = 1;
    samplerDesc.label = label;
    return samplerDesc;
}

static TextureViewDescriptor make_textureViewDesc(const char* label = "")
{
    TextureViewDescriptor textureViewDesc = {};
    textureViewDesc.aspect = TextureAspect::All;
    textureViewDesc.baseArrayLayer = 0;
    textureViewDesc.arrayLayerCount = 1;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.dimension = TextureViewDimension::e1D;
    textureViewDesc.format = TextureFormat::RGBA8Unorm;
    textureViewDesc.label = label;
    return textureViewDesc;
}

static TextureDescriptor make_texture_descriptor(Device& device, uint32_t width, uint32_t height, const char* label = "")
{
    TextureDescriptor textureDesc = {};
    textureDesc.dimension = TextureDimension::e1D;
    textureDesc.size = { width, height, 1 };
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = TextureFormat::RGBA8Unorm;
    textureDesc.usage = TextureUsage::TextureBinding | TextureUsage::CopyDst;
    textureDesc.viewFormats = nullptr;
    textureDesc.viewFormatCount = 0;
    textureDesc.label = label;
    return textureDesc;
}

static TextureDescriptor make_texture_descriptor(Device& device, uint32_t length, const char* label = "")
{
    TextureDescriptor textureDesc = {};
    textureDesc.dimension = TextureDimension::e1D;
    textureDesc.size = { length, 1, 1 };
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = TextureFormat::RGBA8Unorm;
    textureDesc.usage = TextureUsage::TextureBinding | TextureUsage::CopyDst;
    textureDesc.viewFormats = nullptr;
    textureDesc.viewFormatCount = 0;
    textureDesc.label = label;
    return textureDesc;
}

static TextureDescriptor make_texture_descriptor_2d(Device& device, uint32_t width, uint32_t height, const char* label = "")
{
    TextureDescriptor textureDesc = {};
    textureDesc.dimension = TextureDimension::e2D;
    textureDesc.size = { width, height, 1 };
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = TextureFormat::RGBA8Unorm;
    textureDesc.usage = TextureUsage::TextureBinding | TextureUsage::CopyDst;
    textureDesc.viewFormats = nullptr;
    textureDesc.viewFormatCount = 0;
    textureDesc.label = label;
    return textureDesc;
}

static TextureDescriptor make_output_texture_descriptor(Device& device, uint32_t width, uint32_t height, const char* label = "")
{
    TextureDescriptor textureDesc = {};
    textureDesc.dimension = TextureDimension::e2D;
    textureDesc.size = { width, height, 1 };
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = TextureFormat::RGBA8Unorm;
    textureDesc.usage = TextureUsage::TextureBinding | TextureUsage::StorageBinding | TextureUsage::CopySrc | TextureUsage::CopyDst | TextureUsage::RenderAttachment;
    textureDesc.viewFormats = nullptr;
    textureDesc.viewFormatCount = 0;
    textureDesc.label = label;
    return textureDesc;
}

static SamplerDescriptor make_sampler_desc(Device& device, const char* label = "")
{
    SamplerDescriptor samplerDesc = {};
    samplerDesc.addressModeU = AddressMode::ClampToEdge;
    samplerDesc.addressModeV = AddressMode::ClampToEdge;
    samplerDesc.addressModeW = AddressMode::ClampToEdge;
    samplerDesc.magFilter = FilterMode::Linear;
    samplerDesc.minFilter = FilterMode::Linear;
    samplerDesc.mipmapFilter = MipmapFilterMode::Linear;
    samplerDesc.lodMinClamp = 0.0f;
    samplerDesc.lodMaxClamp = 1.0f;
    samplerDesc.compare = CompareFunction::Undefined;
    samplerDesc.maxAnisotropy = 1;
    samplerDesc.label = label;
    return samplerDesc;
}

template <class T>
static Buffer make_buffer(Device& device, size_t size, T usage, const char* label = "")
{
    BufferDescriptor bufferDesc = {};
    bufferDesc.mappedAtCreation = false;
    bufferDesc.size = size;
    bufferDesc.usage = usage;
    bufferDesc.label = label;
    return device.CreateBuffer(&bufferDesc);
}

static CommandEncoder make_encoder(Device& device)
{
    CommandEncoderDescriptor encoderDesc = {};
    encoderDesc.nextInChain = nullptr;
    encoderDesc.label = "Moti command encoder";

    return device.CreateCommandEncoder(&encoderDesc);
}

static ComputePassEncoder begin_compute_pass(CommandEncoder& encoder, const char* label = "")
{
    ComputePassDescriptor computePassDesc = {};
    computePassDesc.timestampWrites = nullptr;
    computePassDesc.label = label;

    return encoder.BeginComputePass(&computePassDesc);
}

static RenderPassEncoder begin_render_pass(CommandEncoder& encoder, TextureView textureView, const char* label = "")
{
    RenderPassColorAttachment attachment {};
    attachment.view = textureView;
    attachment.loadOp = LoadOp::Clear;
    attachment.storeOp = StoreOp::Store;

    RenderPassDescriptor renderpass {};
    renderpass.colorAttachmentCount = 1,
    renderpass.colorAttachments = &attachment;
    renderpass.label = label;
    return encoder.BeginRenderPass(&renderpass);
}

static ComputePipeline make_compute_pipeline(Device& device, ShaderModule& shaderMod, const std::vector<BindGroupLayout>& bindGroupLayouts, const char* entryPoint, const char* label = "")
{
    ComputePipelineDescriptor computePipelineDesc = {};
    computePipelineDesc.compute.entryPoint = entryPoint;
    computePipelineDesc.compute.module = shaderMod;
    computePipelineDesc.label = label;

    PipelineLayoutDescriptor pipelineLayoutDesc = {};
    pipelineLayoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
    pipelineLayoutDesc.bindGroupLayouts = bindGroupLayouts.data();
    pipelineLayoutDesc.label = label;
    computePipelineDesc.layout = device.CreatePipelineLayout(&pipelineLayoutDesc);

    return device.CreateComputePipeline(&computePipelineDesc);
}

// The pipeline, from a caller's render_config. Everything the config does not
// mention keeps the value this library used when all of it was hardcoded.
static TextureFormat to_wgpu(dawn_wrapper::texture_format f)
{
    switch (f) {
    case dawn_wrapper::texture_format::rgba8unorm:
        return TextureFormat::RGBA8Unorm;
    case dawn_wrapper::texture_format::bgra8unorm:
    default:
        return TextureFormat::BGRA8Unorm;
    }
}

static PrimitiveTopology to_wgpu(dawn_wrapper::topology t)
{
    switch (t) {
    case dawn_wrapper::topology::triangle_strip:
        return PrimitiveTopology::TriangleStrip;
    case dawn_wrapper::topology::point_list:
        return PrimitiveTopology::PointList;
    case dawn_wrapper::topology::line_list:
        return PrimitiveTopology::LineList;
    case dawn_wrapper::topology::triangle_list:
    default:
        return PrimitiveTopology::TriangleList;
    }
}

// Premultiplied-free, straightforward source-over for `alpha`, and src + dst for
// `additive`. `replace` leaves the target state without a blend entirely, which is
// what a pipeline had before this was configurable.
static BlendState to_wgpu(dawn_wrapper::blend_mode mode)
{
    BlendState blend {};
    if (mode == dawn_wrapper::blend_mode::additive) {
        blend.color = { BlendOperation::Add, BlendFactor::One, BlendFactor::One };
        blend.alpha = { BlendOperation::Add, BlendFactor::One, BlendFactor::One };
    } else {
        blend.color = { BlendOperation::Add, BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha };
        blend.alpha = { BlendOperation::Add, BlendFactor::One, BlendFactor::OneMinusSrcAlpha };
    }
    return blend;
}

static RenderPipeline make_render_pipeline(Device& device,
    const std::vector<BindGroupLayout>& bindGroupLayouts,
    ShaderModule& fragmentModule, ShaderModule& vertexModule,
    const char* entryPoint, const dawn_wrapper::render_config& config,
    const char* label = "")
{
    const auto blendState = to_wgpu(config.blend);

    ColorTargetState colorTargetState {};
    colorTargetState.format = to_wgpu(config.target);
    if (config.blend != dawn_wrapper::blend_mode::replace) {
        colorTargetState.blend = &blendState;
    }

    FragmentState fragmentState {};
    fragmentState.module = fragmentModule;
    fragmentState.entryPoint = entryPoint;
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTargetState;

    // The built-in layout, used only when the caller wants a vertex buffer. A
    // vertex stage that builds its own positions from @builtin(vertex_index) and a
    // storage buffer binds nothing here.
    VertexAttribute vertexAttrib = {};
    vertexAttrib.shaderLocation = 0;
    vertexAttrib.format = VertexFormat::Float32x2;
    vertexAttrib.offset = 0;

    VertexBufferLayout vertexBufferLayout = {};
    vertexBufferLayout.attributeCount = 1;
    vertexBufferLayout.attributes = &vertexAttrib;
    vertexBufferLayout.arrayStride = 2 * sizeof(float);
    vertexBufferLayout.stepMode = VertexStepMode::Vertex;

    VertexState vertexState {};
    vertexState.module = vertexModule;
    vertexState.entryPoint = config.vertex_entry.c_str();
    if (config.vertex_buffer) {
        vertexState.bufferCount = 1;
        vertexState.buffers = &vertexBufferLayout;
    }

    RenderPipelineDescriptor descriptor {};
    descriptor.vertex = vertexState;
    descriptor.fragment = &fragmentState;
    descriptor.primitive.topology = to_wgpu(config.primitive);
    descriptor.label = label;

    if (!bindGroupLayouts.empty()) {
        PipelineLayoutDescriptor pipelineLayoutDesc = {};
        pipelineLayoutDesc.bindGroupLayoutCount = bindGroupLayouts.size();
        pipelineLayoutDesc.bindGroupLayouts = bindGroupLayouts.data();
        pipelineLayoutDesc.label = label;
        descriptor.layout = device.CreatePipelineLayout(&pipelineLayoutDesc);
    }

    return device.CreateRenderPipeline(&descriptor);
}

static BindGroupLayoutEntry make_bindGroupLayoutBufferEntry(uint32_t binding, BufferBindingType type, ShaderStage stage, const char* label = "")
{
    BindGroupLayoutEntry entry {};
    entry.binding = binding;
    entry.buffer.type = type;
    entry.visibility = stage;
    return entry;
}

static ShaderModule make_compute_shader(Device& device, std::string shaderCode, const char* label = "")
{
    ShaderSourceWGSL wgsl_shaderModuleDesc = {};
    wgsl_shaderModuleDesc.code = shaderCode.c_str();

    ShaderModuleDescriptor shaderModuleDesc = {};
    shaderModuleDesc.nextInChain = &wgsl_shaderModuleDesc;
    shaderModuleDesc.label = label;

    return device.CreateShaderModule(&shaderModuleDesc);
}

static BindGroupEntry make_bindGroupBufferEntry(uint32_t binding, Buffer buffer, uint64_t size, const char* label = "")
{
    BindGroupEntry entry {};
    entry.binding = binding;
    entry.buffer = buffer;
    entry.size = size;
    entry.offset = 0;
    return entry;
}

static BindGroupEntry make_bindGroupBufferEntry(uint32_t binding, Buffer buffer, const char* label = "")
{
    return make_bindGroupBufferEntry(binding, buffer, buffer.GetSize(), label);
}

static Sampler make_sampler(Device& device, AddressMode mode, const char* label = "") // = AddressMode::Repeat)
{
    SamplerDescriptor samplerDesc = dawn_utils::make_samplerDesc(mode, label);
    return device.CreateSampler(&samplerDesc);
}

static BindGroupEntry make_bind_group_entry(unsigned binding, Sampler sampler, const char* label = "")
{
    BindGroupEntry entry = {};
    entry.binding = binding;
    entry.sampler = sampler;
    return entry;
}

static BindGroupEntry make_bind_group_entry(unsigned binding, TextureView view, const char* label = "")
{
    BindGroupEntry entry = {};
    entry.binding = binding;
    entry.textureView = view;
    return entry;
}

static ShaderModule make_shader(Device& device, std::string shaderCode, const char* label = "")
{
    ASSERT(device);
    ShaderSourceWGSL wgslDesc {};
    wgslDesc.code = shaderCode.c_str();

    ShaderModuleDescriptor shaderModuleDescriptor {};
    shaderModuleDescriptor.nextInChain = &wgslDesc;
    shaderModuleDescriptor.label = label;
    return device.CreateShaderModule(&shaderModuleDescriptor);
}

} // dawn_plugin
