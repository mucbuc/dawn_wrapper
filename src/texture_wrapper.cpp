#include <iostream>
#include <map>
#include <memory>

#include "texture_wrapper_impl.hpp"

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

void texture_wrapper::write_region(unsigned x, unsigned y, unsigned width, unsigned height, const std::vector<uint8_t>& rgba)
{
    m_pimpl->write_region(x, y, width, height, rgba);
}

void texture_wrapper::make_sampler(bool clamp_to_edge)
{
    m_pimpl->make_sampler(clamp_to_edge);
}

unsigned texture_wrapper::get_width() const
{
    return m_pimpl->get_width();
}

unsigned texture_wrapper::get_height() const
{
    return m_pimpl->get_height();
}

bool texture_wrapper::is_valid() const
{
    return m_pimpl ? true : false;
}

} // dawn_wrapper
