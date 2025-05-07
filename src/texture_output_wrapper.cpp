#include <iostream>
#include <map>
#include <memory>

#include "texture_output_wrapper_impl.hpp"

struct GLFWwindow;

namespace dawn_wrapper {
texture_output_wrapper::texture_output_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

void texture_output_wrapper::make_sampler(bool clamp_to_edge)
{
    m_pimpl->make_sampler(clamp_to_edge);
}

texture_output_wrapper::operator bool() const
{
    return m_pimpl ? true : false;
}

} // dawn_wrapper
