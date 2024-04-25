#include <iostream>
#include <map>
#include <memory>

#include "encoder_wrapper_impl.h"

namespace dawn_wrapper {
encoder_wrapper::encoder_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

void encoder_wrapper::submit_command_buffer()
{
    m_pimpl->submit_command_buffer();
}

void encoder_wrapper::copy_buffer_to_buffer(buffer_wrapper lhs, buffer_wrapper rhs)
{
    m_pimpl->copy_buffer_to_buffer(lhs, rhs);
}

} // dawn_wrapper
