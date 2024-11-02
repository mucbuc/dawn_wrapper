#include <iostream>
#include <map>
#include <memory>

#include "buffer_wrapper_impl.h"
#include "dawn_utils.hpp"
#include "dawn_wrapper.h"

struct GLFWwindow;

namespace dawn_wrapper {
buffer_wrapper::buffer_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

buffer_wrapper & buffer_wrapper::write(const std::vector<uint8_t>& colors)
{
    if (m_pimpl) {
        m_pimpl->write(colors);
    }
    return * this;
}

buffer_wrapper & buffer_wrapper::write(void* p, size_t size)
{
    if (m_pimpl) {
        m_pimpl->write(p, size);
    }
    return * this;
}

buffer_wrapper & buffer_wrapper::get_output(std::function<void(unsigned, const void*)> cb)
{
    if (m_pimpl) {
        m_pimpl->get_output(cb);
    }
    return * this;
}

bool buffer_wrapper::done()
{
    return m_pimpl && m_pimpl->done();
}

buffer_wrapper::operator bool() const
{
    return m_pimpl ? true : false;
}

size_t buffer_wrapper::get_size()
{
    return m_pimpl ? m_pimpl->get_size() : 0;
}

} // dawn_wrapper
