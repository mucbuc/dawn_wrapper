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

void texture_output_wrapper::write(const std::vector<uint8_t>& colors)
{
    m_pimpl->write(colors);
}

void texture_output_wrapper::write_region(unsigned x, unsigned y, unsigned width, unsigned height, const std::vector<uint8_t>& rgba)
{
    m_pimpl->write_region(x, y, width, height, rgba);
}

void texture_output_wrapper::make_sampler(bool clamp_to_edge)
{
    m_pimpl->make_sampler(clamp_to_edge);
}

unsigned texture_output_wrapper::get_width() const
{
    return m_pimpl ? m_pimpl->get_width() : 0;
}

unsigned texture_output_wrapper::get_height() const
{
    return m_pimpl ? m_pimpl->get_height() : 0;
}

size_t texture_output_wrapper::bytes_per_row() const
{
    return m_pimpl ? m_pimpl->bytes_per_row() : 0;
}

size_t texture_output_wrapper::readback_size() const
{
    return m_pimpl ? m_pimpl->bytes_per_row() * m_pimpl->get_height() : 0;
}

bool texture_output_wrapper::is_valid() const
{
    return m_pimpl ? true : false;
}

} // dawn_wrapper
