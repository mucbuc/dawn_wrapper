#include <iostream>
#include <map>
#include <memory>

#include "buffer_wrapper_impl.h"

struct GLFWwindow;

namespace dawn_wrapper {
buffer_wrapper::buffer_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

void buffer_wrapper::write(const std::vector<uint8_t>& colors)
{
    if (m_pimpl) {
        m_pimpl->write(colors);
    }
}

void buffer_wrapper::write(void* p, size_t size)
{
    if (m_pimpl) {
        m_pimpl->write(p, size);
    }
}

void buffer_wrapper::get_output(std::function<void(unsigned, const void*)> cb)
{
    if (m_pimpl) {
        m_pimpl->get_output(cb);
    }
}

bool buffer_wrapper::done()
{
    return m_pimpl && m_pimpl->done();
}

buffer_wrapper::operator bool() const
{
    return m_pimpl ? true : false;
}

} // dawn_wrapper
