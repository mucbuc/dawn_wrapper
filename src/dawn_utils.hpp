#pragma once

#include <cassert>
#include <chrono>
#include <ctime>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <webgpu/webgpu_cpp.h>
#include <webgpu/webgpu_glfw.h>

#ifndef ASSERT
#define ASSERT(p) assert((p))
#endif

using namespace std;
using namespace wgpu;

namespace dawn_utils {

static void write_texture(Device& device, Texture& texture, TextureDescriptor& desc, std::vector<uint8_t> colorTexture)
{
    ImageCopyTexture destination {};
    destination.texture = texture;
    destination.mipLevel = 0;
    destination.origin = { 0, 0, 0 };
    destination.aspect = TextureAspect::All;

    TextureDataLayout source {};
    source.offset = 0;
    source.bytesPerRow = 4 * desc.size.width;
    source.rowsPerImage = desc.size.height;

    device.GetQueue().WriteTexture(&destination, colorTexture.data(), colorTexture.size(), &source, &desc.size);
}

static BindGroup make_bindGroup(Device& device, BindGroupLayout& layout, const std::vector<BindGroupEntry>& entries)
{
    BindGroupDescriptor bindGroupDesc;
    bindGroupDesc.entryCount = entries.size();
    bindGroupDesc.entries = entries.data();
    bindGroupDesc.layout = layout;
    return device.CreateBindGroup(&bindGroupDesc);
}

template <class... T>
static BindGroup make_bindGroup(Device& device, BindGroupLayout& layout, T... entriesIntializer)
{
    std::vector<BindGroupEntry> entries { entriesIntializer... };
    return make_bindGroup(device, layout, entries);
}

static BindGroupLayout make_bindGroupLayout(Device& device, const std::vector<BindGroupLayoutEntry>& entries)
{
    ASSERT(!entries.empty());

    BindGroupLayoutDescriptor bindGroupLayoutDesc = {};
    bindGroupLayoutDesc.entryCount = entries.size();
    bindGroupLayoutDesc.entries = entries.data();
    return device.CreateBindGroupLayout(&bindGroupLayoutDesc);
}

template <class... T>
static BindGroupLayout make_bindGroupLayout(Device& device, T... entriesIntializer)
{
    std::vector<BindGroupLayoutEntry> entries { entriesIntializer... };
    return make_bindGroupLayout(device, entries);
}

static SamplerDescriptor make_samplerDesc(AddressMode mode = AddressMode::Repeat)
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
    return samplerDesc;
}

static TextureViewDescriptor make_textureViewDesc()
{
    TextureViewDescriptor textureViewDesc = {};
    textureViewDesc.aspect = TextureAspect::All;
    textureViewDesc.baseArrayLayer = 0;
    textureViewDesc.arrayLayerCount = 1;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.dimension = TextureViewDimension::e1D;
    textureViewDesc.format = TextureFormat::RGBA8Unorm;
    return textureViewDesc;
}

static TextureDescriptor make_texture_descriptor(Device& device, uint32_t width, uint32_t height)
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
    return textureDesc;
}

static TextureDescriptor make_texture_descriptor(Device& device, uint32_t length)
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
    return textureDesc;
}

static TextureDescriptor make_output_texture_descriptor(Device& device, uint32_t width, uint32_t height)
{
    TextureDescriptor textureDesc = {};
    textureDesc.dimension = TextureDimension::e1D;
    textureDesc.size = { width, height, 1 };
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = TextureFormat::RGBA8Unorm;
    textureDesc.usage = TextureUsage::TextureBinding | TextureUsage::StorageBinding | TextureUsage::CopySrc | TextureUsage::CopyDst;
    textureDesc.viewFormats = nullptr;
    textureDesc.viewFormatCount = 0;
    return textureDesc;
}

static SamplerDescriptor make_sampler_desc(Device& device)
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
    return samplerDesc;
}

template <class T>
static Buffer make_buffer(Device& device, size_t size, T usage)
{
    BufferDescriptor bufferDesc = {};
    bufferDesc.mappedAtCreation = false;
    bufferDesc.size = size;
    bufferDesc.usage = usage;
    return device.CreateBuffer(&bufferDesc);
}

static CommandEncoder make_encoder(Device& device)
{
    CommandEncoderDescriptor encoderDesc = {};
    encoderDesc.nextInChain = nullptr;
    encoderDesc.label = "Moti command encoder";

    return device.CreateCommandEncoder(&encoderDesc);
}

static ComputePassEncoder begin_compute_pass(CommandEncoder& encoder)
{
    ComputePassDescriptor computePassDesc = {};
    computePassDesc.timestampWrites = nullptr;

    return encoder.BeginComputePass(&computePassDesc);
}

static RenderPassEncoder begin_render_pass(CommandEncoder& encoder, TextureView textureView)
{
    RenderPassColorAttachment attachment {};
    attachment.view = textureView;
    attachment.loadOp = LoadOp::Clear;
    attachment.storeOp = StoreOp::Store;

    RenderPassDescriptor renderpass {};
    renderpass.colorAttachmentCount = 1,
    renderpass.colorAttachments = &attachment;
    return encoder.BeginRenderPass(&renderpass);
}

static ComputePipeline make_compute_pipeline(Device& device, ShaderModule& shaderMod, BindGroupLayout& bindGroupLayout, const char* entryPoint)
{
    ComputePipelineDescriptor computePipelineDesc = {};
    computePipelineDesc.compute.entryPoint = entryPoint;
    computePipelineDesc.compute.module = shaderMod;

    PipelineLayoutDescriptor pipelineLayoutDesc = {};
    pipelineLayoutDesc.bindGroupLayoutCount = 1;
    pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
    computePipelineDesc.layout = device.CreatePipelineLayout(&pipelineLayoutDesc);

    return device.CreateComputePipeline(&computePipelineDesc);
}

static RenderPipeline make_render_pipeline(Device& device, BindGroupLayout& bindGroupLayout, ShaderModule& fragmentModule, ShaderModule& vertexModule, const char* entryPoint)
{
    ColorTargetState colorTargetState {};
    colorTargetState.format = TextureFormat::BGRA8Unorm;

    FragmentState fragmentState {};
    fragmentState.module = fragmentModule;
    fragmentState.entryPoint = entryPoint;
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTargetState;

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
    vertexState.entryPoint = "vertexMain";

    PipelineLayoutDescriptor pipelineLayoutDesc = {};
    pipelineLayoutDesc.bindGroupLayoutCount = 1;
    BindGroupLayout layouts[] = { bindGroupLayout };
    pipelineLayoutDesc.bindGroupLayouts = layouts;

    RenderPipelineDescriptor descriptor {};
    descriptor.vertex = vertexState;
    descriptor.fragment = &fragmentState;
    descriptor.vertex.bufferCount = 1;
    descriptor.vertex.buffers = &vertexBufferLayout;
    descriptor.layout = device.CreatePipelineLayout(&pipelineLayoutDesc);

    return device.CreateRenderPipeline(&descriptor);
}

static RenderPipeline make_render_pipeline(Device& device, ShaderModule& fragmentModule, ShaderModule& vertexModule, const char* entryPoint)
{
    ColorTargetState colorTargetState {};
    colorTargetState.format = TextureFormat::BGRA8Unorm;

    FragmentState fragmentState {};
    fragmentState.module = fragmentModule;
    fragmentState.entryPoint = entryPoint;
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTargetState;

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
    vertexState.entryPoint = "vertexMain";

    PipelineLayoutDescriptor pipelineLayoutDesc = {};
    pipelineLayoutDesc.bindGroupLayoutCount = 0;

    RenderPipelineDescriptor descriptor {};
    descriptor.vertex = vertexState;
    descriptor.fragment = &fragmentState;
    descriptor.vertex.bufferCount = 1;
    descriptor.vertex.buffers = &vertexBufferLayout;
    descriptor.layout = device.CreatePipelineLayout(&pipelineLayoutDesc);

    return device.CreateRenderPipeline(&descriptor);
}

static SwapChain make_swap_chain(Device& device, wgpu::Surface surface, unsigned width, unsigned height)
{
    // ASSERT(surface);

    SwapChainDescriptor swapChainDesc {};
    swapChainDesc.usage = TextureUsage::RenderAttachment;
    swapChainDesc.format = TextureFormat::BGRA8Unorm;
    swapChainDesc.width = (unsigned int)width;
    swapChainDesc.height = (unsigned int)height;
    swapChainDesc.presentMode = PresentMode::Fifo;

    return device.CreateSwapChain(surface, &swapChainDesc);
}

static BindGroupLayoutEntry make_bindGroupLayoutBufferEntry(uint32_t binding, BufferBindingType type, ShaderStage stage)
{

    BindGroupLayoutEntry entry {};
    entry.binding = binding;
    entry.buffer.type = type;
    entry.visibility = stage;
    return entry;
}

static ShaderModule make_compute_shader(Device& device, std::string shaderCode)
{
    ShaderModuleWGSLDescriptor wgsl_shaderModuleDesc = {};

    wgsl_shaderModuleDesc.code = shaderCode.c_str();

    ShaderModuleDescriptor shaderModuleDesc = {};
    shaderModuleDesc.nextInChain = &wgsl_shaderModuleDesc;

    return device.CreateShaderModule(&shaderModuleDesc);
}

static BindGroupEntry make_bindGroupBufferEntry(uint32_t binding, Buffer buffer, uint32_t size)
{
    BindGroupEntry entry {};
    entry.binding = binding;
    entry.buffer = buffer;
    entry.size = size;
    entry.offset = 0;
    return entry;
}

static BindGroupEntry make_bindGroupBufferEntry(uint32_t binding, Buffer buffer)
{
    return make_bindGroupBufferEntry(binding, buffer, buffer.GetSize());
}

static Sampler make_sampler(Device& device, AddressMode mode = AddressMode::Repeat)
{
    SamplerDescriptor samplerDesc = dawn_utils::make_samplerDesc(mode);
    return device.CreateSampler(&samplerDesc);
}

static BindGroupEntry make_bind_group_entry(unsigned binding, Sampler sampler)
{
    BindGroupEntry entry = {};
    entry.binding = binding;
    entry.sampler = sampler;
    return entry;
}

static BindGroupEntry make_bind_group_entry(unsigned binding, TextureView view)
{
    BindGroupEntry entry = {};
    entry.binding = binding;
    entry.textureView = view;
    return entry;
}

static ShaderModule make_shader(Device& device, std::string shaderCode)
{
    ShaderModuleWGSLDescriptor wgslDesc {};
    wgslDesc.code = shaderCode.c_str();

    ShaderModuleDescriptor shaderModuleDescriptor {};
    shaderModuleDescriptor.nextInChain = &wgslDesc;
    return device.CreateShaderModule(&shaderModuleDescriptor);
}

static std::string read_file(std::string path)
{

    std::ifstream file(path);
    std::string code;

    while (file) {
        std::string line;
        std::getline(file, line);
        code.append(line + "\n");
    }

    return code;
}

static std::string read_shader(std::string path)
{
    std::string shared = read_file("../Resources/shared.wgsl");
    return shared + read_file("../Resources/" + path);
}

static std::string read_shader(std::string path1, std::string path2)
{
    std::string shared = read_file("../Resources/shared.wgsl");
    return shared + read_file("../Resources/" + path1) + read_file("../Resources/" + path2);
}

template <class T>
static std::array<uint8_t, 4> get_colors(T hexColors)
{
    uint8_t red = stoi(hexColors.first.substr(1, 2), 0, 16);
    uint8_t green = stoi(hexColors.first.substr(3, 2), 0, 16);
    uint8_t blue = stoi(hexColors.first.substr(5, 2), 0, 16);
    uint8_t alpha = hexColors.second * 255;
    return std::array<uint8_t, 4>({ red, green, blue, alpha });
}

static std::vector<uint8_t> getDrawColors(std::map<float, std::pair<std::string, float>> layers, bool blend)
{
    ASSERT(!layers.empty());

    auto last = layers.end();
    std::advance(last, -1);
    std::array<uint8_t, 4> previous_color = { get_colors(last->second) };
    unsigned previous_distance = 0; // last->first;

    std::pair<float, float> distance_range = std::make_pair(layers.begin()->first, last->first);

    const auto size = 1 + distance_range.second - distance_range.first;
    std::vector<uint8_t> colors(4 * size, 0);

    static const double pi = acos(-1);
    for (auto layer : layers) {
        const auto rgba = get_colors(layer.second);

        float weight = 1;

        for (auto pixel = previous_distance; pixel < layer.first; ++pixel) {
            if (blend) {
                weight = (pixel - previous_distance) / (layer.first - previous_distance);
                // weight = 0.5 - 0.5 * cos(pi * weight);
            }
            float om_weight = 1 - weight;

            colors[4 * pixel] = rgba[0] * weight + previous_color[0] * om_weight;
            colors[4 * pixel + 1] = rgba[1] * weight + previous_color[1] * om_weight;
            colors[4 * pixel + 2] = rgba[2] * weight + previous_color[2] * om_weight;
            colors[4 * pixel + 3] = rgba[3] * weight + previous_color[3] * om_weight;
        }

        previous_distance = layer.first;
        previous_color = rgba;
    }

    return colors;
}

} // dawn_plugin
