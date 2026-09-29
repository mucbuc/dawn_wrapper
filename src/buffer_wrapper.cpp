#include <iostream>
#include <map>
#include <memory>

#include "buffer_wrapper_impl.hpp"

struct GLFWwindow;

namespace dawn_wrapper {
buffer_wrapper::buffer_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

buffer_wrapper& buffer_wrapper::write(const std::vector<uint8_t>& colors)
{
    if (m_pimpl) {
        m_pimpl->write(colors);
    }
    return *this;
}

buffer_wrapper& buffer_wrapper::write(const void* p)
{
    if (m_pimpl) {
        m_pimpl->write(p);
    }
    return *this;
}

buffer_wrapper& buffer_wrapper::get_output(std::function<void(size_t, const void*)> cb)
{
    if (m_pimpl) {
        m_pimpl->get_output(cb);
    }
    return *this;
}

bool buffer_wrapper::done() const
{
    return m_pimpl && m_pimpl->done();
}

bool buffer_wrapper::is_valid() const
{
    return m_pimpl ? true : false;
}

size_t buffer_wrapper::get_size() const
{
    return m_pimpl ? m_pimpl->get_size() : 0;
}

buffer_usage buffer_wrapper::get_usage() const
{
    if (!m_pimpl) {
        return {};
    }
    const auto usage = m_pimpl->m_usage;
    const auto has = [usage](BufferUsage bit) { return (usage & bit) != BufferUsage::None; };
    return {
        .copy_src = has(BufferUsage::CopySrc),
        .copy_dst = has(BufferUsage::CopyDst),
        .storage = has(BufferUsage::Storage),
        .uniform = has(BufferUsage::Uniform),
        .index = has(BufferUsage::Index),
        .vertex = has(BufferUsage::Vertex),
        .map_read = has(BufferUsage::MapRead),
    };
}

} // dawn_wrapper
