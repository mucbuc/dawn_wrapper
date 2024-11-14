#include <iostream>
#include <map>
#include <memory>

#include "texture_wrapper_impl.h"

struct GLFWwindow;

namespace dawn_wrapper {
texture_wrapper::texture_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

void texture_wrapper::write(const std::vector<uint8_t>& colors)
{
    m_pimpl->write(colors);
}

void texture_wrapper::make_sampler(bool clamp_to_edge)
{
    m_pimpl->make_sampler(clamp_to_edge);
}

texture_wrapper::operator bool() const
{
    return m_pimpl ? true : false;
}

} // dawn_wrapper
