#include <iostream>
#include <map>
#include <memory>

#include "encoder_wrapper_impl.hpp"

namespace dawn_wrapper {
encoder_wrapper::encoder_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

encoder_wrapper& encoder_wrapper::submit_command_buffer()
{
    m_pimpl->submit_command_buffer();
    return *this;
}

encoder_wrapper& encoder_wrapper::copy_buffer_to_buffer(buffer_wrapper source, buffer_wrapper dest, size_t offset)
{
    m_pimpl->copy_buffer_to_buffer(source, dest, offset);
    return *this;
}

encoder_wrapper& encoder_wrapper::copy_texture_to_buffer(texture_output_wrapper texture, buffer_wrapper destination)
{
    m_pimpl->copy_texture_to_buffer(texture, destination);
    return *this;
}

} // dawn_wrapper
