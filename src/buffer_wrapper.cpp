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
    m_pimpl->write(colors);
}

void buffer_wrapper::write(void* p, size_t size)
{
    m_pimpl->write(p, size);
}

} // dawn_wrapper
